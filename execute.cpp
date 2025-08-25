#include <iostream>
#include <bitset>
#include <string>
#include <cstring>
#include <vector>

#include "askapi.h"
#include "logic.h"

using namespace std;

instr_parts instr_p;        // a struct containing the different parts 
ask_mode_t bank = am_sys;   // current register bank
bool isRunning = true;      // boolean variable that says if the cpu is running or not

// Getter/setter for the isRunning variable
bool get_running(void) {
    return isRunning;
}

void set_running(bool new_isRunning) {
    isRunning = new_isRunning;
}

ask_mode_t ask_bank_get() {
    bitset<5> mode(ask_cpsr_get() & 0x11111);

    if (mode == 0x11111) {
        bank = am_sys;
    } else if (mode == 0x10011) {
        bank = am_svc;
    }

    return bank;
}

/**
 * @brief Converts a bitset to an int
 * 
 * @param val the value to convert to an int
 * @return int 
 */
template <size_t N>
int btoi(const bitset<N>& val) {
    int result = 0;
    for (size_t i = N; i > 0; --i) {
        result = (result << 1) | val[i - 1];
    }

    return result;
}

/**
 * @brief Converts a bitset of size N to a b_word
 * 
 * @param val the value to convert to a b_word
 * @return b_word 
 */
template <size_t N>
b_word btob(const bitset<N>& val) {
    b_word newVal(0);
    for (int i = 0; i < val.size(); ++i) {
        newVal[i] = val[i];
    }

    return newVal;
}

/**
 * @brief Shifts the value in a register and returns the shifted value
 * 
 * @param mode the type of shift
 * @param reg the source register of the number being shifted
 * @param shift_imm the value of the shift
 * @return b_word 
 */
b_word shift_num_immed(shift mode, reg reg, shift_val shift_imm) {
    b_word shift_num = 0; // the new shifted number

    // shift type used
    if (mode == 0b00) { // lsl/asl
        shift_num = ask_reg_get(bank, btoi(reg)) << btoi(shift_imm);        
    } else if (mode == 0b01) { // lsr
        shift_num = ask_reg_get(bank, btoi(reg)) >> btoi(shift_imm);
    } else if (mode == 0b10) { // asr
        shift_num = (sword) ask_reg_get(bank, btoi(reg)) >> btoi(shift_imm);
    } else if (mode == 0b11) { // ror (and rrx eventually?)
        word Rm_val = ask_reg_get(bank, btoi(reg));
        shift_num = (Rm_val >> btoi(shift_imm)) | Rm_val << (32 - btoi(shift_imm));
    }
    return shift_num;
}

/**
 * @brief Gets the number used in an operation from the `shifter_operand` section of the instruction
 *  
 * @return int 
 */
int data_proc_shift() {
    int operand = 0;

    if (instr_p.i_bit == 0) { // shift
        shift shift(get_bits(instr_p.rest << 25, 2));   // the type of shift
        reg Rm(get_bits(instr_p.rest << 28, 4));        // a source register

        // get the shift_val using andimmediate shift (true-case) or register shift (false-case)
        shift_val shift_val = (get_bits(instr_p.rest << 27, 1) == 0b0) ? get_bits(instr_p.rest << 20, 5) : ask_reg_get(bank, get_bits(instr_p.rest << 20, 4));

        // convert the shift_val to the shifted number based on the shift specified
        b_word shift_num = shift_num_immed(shift, Rm, shift_val); // shifted number 
        operand = btoi(shift_num);

    } else { // 32-bit immediate
        bitset<4> rotate_imm(get_bits(instr_p.rest << 20, 4));  // rotate_imm * 2 = number of times to rotate
        bitset<8> immed_8(get_bits(instr_p.rest << 24, 8));     // immediate to shift
        if (rotate_imm == 0) { // no shift 
            operand = btoi(immed_8);
        } else { // shift
            int rotate = btoi(rotate_imm) * 2;  // convert to int for rotate amount
            b_word immed_32 = btob(immed_8);    // 32-bit number to copy shifted number into
            immed_32 = (immed_32 >> rotate) | (immed_32 << (32 - rotate)); // rotating logic 
            operand = btoi(immed_32);
        }
    }
    return operand;
}

/**
 * @brief 
 * 
 * @param isByte are we loading/storing a byte or word
 * @param isLdr is this a load or a store
 * @param Rd destination register
 * @param address address to load/store
 */
void check_instr_type(bool isByte, bool isLdr, reg Rd, word address) {
    if (isByte && isLdr) {
        ask_reg_set(bank, btoi(Rd), get_host().mem_load(address));
    } else if (!isByte && isLdr) {
        ask_reg_set(bank, btoi(Rd), get_host().mem_load(address));
    } else if (isByte && !isLdr) { 
        get_host().mem_store(address, ask_reg_get(bank, btoi(Rd)) & 0xff);
    } else {
        get_host().mem_store(address, ask_reg_get(bank, btoi(Rd)));
    }
}

/**
 * @brief counts the number of set bits in the register list
 * 
 * @param reg_list the 16-bit "list" of registers
 * @return int 
 */
int count_reg(bitset<16> reg_list) {
    int count = 0;
    while (btoi(reg_list) > 0b0){
        if ((reg_list & (bitset<16>) 1) == 1) {
            count += 1;
        }
        reg_list >>= 1;
    }
    return count;
}

// The main execute function
vector<int> execute(word int_instr) {
    b_word bin_instr = convert_instr(int_instr);
    decode_init(bin_instr, &instr_p);
    char *instr = decode(&instr_p);

    vector<int> btype(2, 0);

    if (strncmp(instr, "mul", 3) == 0) {    // MUL
        // get Rd, Rs, and Rm
        reg Rd(get_bits(instr_p.rest << 12, 4));
        reg Rs(get_bits(instr_p.rest << 20, 4));
        reg Rm(get_bits(instr_p.rest << 28, 4));

        // do the operation and set Rd accordingly
        word quotient = ask_reg_get(bank, btoi(Rm)) * ask_reg_get(bank, btoi(Rs));
        ask_reg_set(bank, btoi(Rd), quotient);

    } else if (strncmp(instr, "and", 3) == 0) {     // AND
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        // get the second operand for the operation
        int op2 = data_proc_shift();

        // do the operation and set Rd accordingly
        word ans = ask_reg_get(bank, btoi(Rn)) & op2;
        ask_reg_set(bank, btoi(Rd), ans);
        
    } else if (strncmp(instr, "cmp", 3) == 0) {     // CMP

        code nzcv = 0b0000;

        // get Rn
        reg Rn(get_bits(instr_p.rest << 12, 4));

        word op1 = ask_reg_get(bank, btoi(Rn)); // value in Rn
        word op2 = data_proc_shift();           // 2nd operand
        word alu_out = op1 - op2;               // result

        bit w = (alu_out & 0x80000000) >> 31;   // first bit in `alu_out`

        /* N */ // negative
            nzcv[3] = (w != 0);     
        /* Z */ // equal
            nzcv[2] = (alu_out == 0);   
        /* C */ // carry      
            nzcv[1] = (op1 >= op2) ? 1 : 0;
        /* V */ // overflow
            sword sop1 = (sword) op1;
            sword sop2 = (sword) op2;
            sword salu_out = (sword) alu_out;

            nzcv[0] = (sop1 > 0 && sop2 < 0 && salu_out <= 0) || (sop1 < 0 && sop2 > 0 && salu_out >= 0);

        word cpsr = ask_cpsr_get();


        b_word w_nzcv = btob(nzcv);

        cpsr |= 0xf0000000;
        cpsr &= ((btoi(nzcv) << 28) | 0x0fffffff);
        ask_cpsr_set(cpsr);

    } else if (strncmp(instr, "eor", 3) == 0) {     // EOR
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        // get the second operand for the operation
        int op2 = data_proc_shift();

        // do the operation and set Rd accordingly
        word ans = ask_reg_get(bank, btoi(Rn)) ^ op2;
        ask_reg_set(bank, btoi(Rd), ans);

    } else if (strncmp(instr, "sub", 3) == 0) {     // SUB
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        // get the second operand for the subtraction
        int op2 = data_proc_shift();

        // do the subtraction and set Rd accordingly
        word ans = ask_reg_get(bank, btoi(Rn)) - op2;
        ask_reg_set(bank, btoi(Rd), ans);

    } else if (strncmp(instr, "add", 3) == 0) {     // ADD
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        // get the second operand for the addition
        int op2 = data_proc_shift();

        // do the addition and set Rd accordingly
        word ans = ask_reg_get(bank, btoi(Rn)) + op2;
        ask_reg_set(bank, btoi(Rd), ans);

    } else if (strncmp(instr, "rsb", 3) == 0) {     // RSB
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        // get the second operand for the subtraction
        int op1 = data_proc_shift();

        // do the subtraction and set Rd accordingly
        word ans = op1 - ask_reg_get(bank, btoi(Rn));
        ask_reg_set(bank, btoi(Rd), ans);

    } else if (strncmp(instr, "bx", 2) == 0) {     // BBX
        reg Rm(get_bits(instr_p.rest << 28, 4));
        word num = ask_reg_get(bank, btoi(Rm)) & 0xfffffffe;

        btype[0] = 2;
        btype[1] = num;

    } else if (strncmp(instr, "orr", 3) == 0) {     // ORR
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        // get the second operand for the operation
        int op2 = data_proc_shift();

        // do the operation and set Rd accordingly
        word ans = ask_reg_get(bank, btoi(Rn)) | op2;
        ask_reg_set(bank, btoi(Rd), ans);

    } else if (strncmp(instr, "mov", 3) == 0) {     // MOV/MOVS (w/ pc)
        // get Rd
        reg Rd(get_bits(instr_p.rest << 16, 4));


        // get value to put in Rd
        int op = data_proc_shift();

        if (instr_p.s_bit == 0b1 && btoi(Rd) == 15) {
            ask_cpsr_set(ask_spsr_get(bank));
        }

        // set Rd to the value
        ask_reg_set(bank, btoi(Rd), op);

    } else if (strncmp(instr, "bic", 3) == 0) {     // BIC
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        // get the second operand for the operation
        int op2 = data_proc_shift();

        // do the operation and set Rd accordingly
        word ans = ask_reg_get(bank, btoi(Rn)) & ~op2;
        ask_reg_set(bank, btoi(Rd), ans);

    } else if (strncmp(instr, "mvn", 3) == 0) {     // MVN
        // get Rd and Rn
        reg Rd(get_bits(instr_p.rest << 16, 4));

        // get the second operand for the operation
        int op = data_proc_shift();

        // do the operation and set Rd accordingly
        ask_reg_set(bank, btoi(Rd), ~op);

    } else if (strncmp(instr, "bl", 2) == 0
            || strncmp(instr, "b", 1) == 0 )  {     // B or BL
        // grab the L bit (its the same bit number as the P bit from ldr/str instructions)
        bit L = instr_p.p_bit; 

        b_word unsigned_immed_32(get_bits(bin_instr << 8, 32));

        int32_t signed_immed_32 = btoi(unsigned_immed_32) >> 8;

        signed_immed_32 <<= 2;

        if (L == 0b1) { // BL
            ask_reg_set(bank, 14, ask_reg_get(bank, 15) - 4);
        }

        btype[0] = 1;
        btype[1] = signed_immed_32;

    } else if (strncmp(instr, "ldr", 3) == 0
            || strncmp(instr, "str", 3) == 0) {     // LDR or STR

        reg Rd(get_bits(instr_p.rest << 16, 4));
        reg Rn(get_bits(instr_p.rest << 12, 4));

        word address = 0;
        // adding or subtracting
        int u = (instr_p.u_bit == 1) ? 1 : -1;

        // ldr/str byte, else word
        bool isByte = (instr_p.b_bit == 1);

        // ldr (= true) or str (= false)
        bool isLdr  = (instr_p.l_bit == 1);

        if (instr_p.p_bit == 0 && instr_p.w_bit == 0) { // post-index addressing
            sword post_index = 0;

            if (instr_p.i_bit == 0) { // immediate offset
                address = ask_reg_get(bank, btoi(Rn));
                post_index = (get_bits(instr_p.rest << 20, 12) * u);
            } else {
                reg Rm(get_bits(instr_p.rest << 28, 4));
                if (get_bits(instr_p.rest << 20, 8) == 0) { // register offset
                    address = ask_reg_get(bank, btoi(Rn));
                    post_index = (ask_reg_get(bank, btoi(Rm)) * u);
                } else { // scaled register offset
                    // the type of shift
                    shift shift(get_bits(instr_p.rest << 25, 2));

                    // get the shift_val
                    shift_val shift_val(get_bits(instr_p.rest << 20, 5)); // specifies the shift number
                    if (shift != 0b00 && shift_val == 0b0) { shift_val = 32; }

                    // convert the shift_val to the shifted number based on the shift specified
                    b_word shift_num = shift_num_immed(shift, Rm, shift_val); // the shifted number to put in Rd

                    address = ask_reg_get(bank, btoi(Rn));
                    post_index = (btoi(shift_num) * u);
                }
            }
            ask_reg_set(bank, btoi(Rn), address + post_index);

        } else if (instr_p.p_bit == 0 && instr_p.w_bit == 1) {
            // LDRBT, LDRT, STRBT, STRT (User mode). Not implemented in current version.
        } else if (instr_p.p_bit == 1 && instr_p.w_bit == 0) { // offset addressing
            if (instr_p.i_bit == 0) { // immediate offset
                address = ask_reg_get(bank, btoi(Rn)) + (get_bits(instr_p.rest << 20, 12) * u);
            } else {
                reg Rm(get_bits(instr_p.rest << 28, 4));
                if (get_bits(instr_p.rest << 20, 8) == 0) { // register offset
                    address = ask_reg_get(bank, btoi(Rn)) + (ask_reg_get(bank, btoi(Rm)) * u);
                } else { // scaled register offset
                    // the type of shift
                    shift shift(get_bits(instr_p.rest << 25, 2));

                    // get the shift_val
                    shift_val shift_val(get_bits(instr_p.rest << 20, 5)); // specifies the shift number
                    if (shift != 0b00 && shift_val == 0b0) { shift_val = 32; }

                    // convert the shift_val to the shifted number based on the shift specified
                    b_word shift_num = shift_num_immed(shift, Rm, shift_val); // the shifted number to put in Rd

                    address = ask_reg_get(bank, btoi(Rn)) + (btoi(shift_num) * u);
                }
            }

        } else if (instr_p.p_bit == 1 && instr_p.w_bit == 1) { // pre-index addressing
            if (instr_p.i_bit == 0) { // immediate offset
                address = ask_reg_get(bank, btoi(Rn)) + (get_bits(instr_p.rest << 20, 12) * u);
            } else {
                reg Rm(get_bits(instr_p.rest << 28, 4));
                if (get_bits(instr_p.rest << 20, 8) == 0) { // register offset
                    address = ask_reg_get(bank, btoi(Rn)) + (ask_reg_get(bank, btoi(Rm)) * u);
                } else { // scaled register offset
                    // the type of shift
                    shift shift(get_bits(instr_p.rest << 25, 2));

                    // get the shift_val
                    shift_val shift_val(get_bits(instr_p.rest << 20, 5)); // specifies the shift number
                    if (shift != 0b00 && shift_val == 0b0) { shift_val = 32; }

                    // convert the shift_val to the shifted number based on the shift specified
                    b_word shift_num = shift_num_immed(shift, Rm, shift_val); // the shifted number to put in Rd

                    address = ask_reg_get(bank, btoi(Rn)) + (btoi(shift_num) * u);
                }
            }
            ask_reg_set(bank, btoi(Rn), address);
        }

        // Check if we are 1) ldr/str and 2) using a word or byte
        // TODO: add check for word-aligned
        // if ((address & 0b11) == 0b0) {
            if (isByte && isLdr) {
                word byte = 0;
                if ((address & 0x3) == 0b11) {
                    byte = (get_host().mem_load(address & 0xfffc) & 0xff000000) >> 24;
                } else if ((address & 0x3) == 0b10) {
                    byte = (get_host().mem_load(address & 0xfffc) & 0x00ff0000) >> 16;
                } else if ((address & 0x3) == 0b01) {
                    byte = (get_host().mem_load(address & 0xfffc) & 0x0000ff00) >> 8;
                } else {
                    byte = get_host().mem_load(address) & 0x000000ff;
                }
                ask_reg_set(bank, btoi(Rd), byte);
            } else if (!isByte && isLdr) {
                ask_reg_set(bank, btoi(Rd), get_host().mem_load(address));
            } else if (isByte && !isLdr) { 
                word mem_word = get_host().mem_load(address & 0xfffc);
                word val; 

                if ((address & 0x3) == 0b11) {
                    mem_word = (mem_word | 0xff000000) & ((ask_reg_get(bank, btoi(Rd)) << 24) | 0x00ffffff);
                } else if ((address & 0x3) == 0b10) {
                    mem_word = (mem_word | 0x00ff0000) & ((ask_reg_get(bank, btoi(Rd)) << 16) | 0xff00ffff);
                } else if ((address & 0x3) == 0b01) {
                    mem_word = (mem_word | 0x0000ff00) & ((ask_reg_get(bank, btoi(Rd)) << 8) | 0xffff00ff);
                } else {
                    mem_word = (mem_word | 0x000000ff) & (ask_reg_get(bank, btoi(Rd)) | 0xffffff00);
                }
                get_host().mem_store((address & 0xfffc), mem_word);

                

            } else {
                get_host().mem_store(address, ask_reg_get(bank, btoi(Rd)));
            }

            // increment the loads/stores variable
            if (isLdr) inc('l'); else inc('s');
        // } 

        

    } else if (strncmp(instr, "ldm", 3) == 0
            || strncmp(instr, "stm", 3) == 0) {     // LDM or STM
        if (instr_p.p_bit == 0 && instr_p.u_bit == 0) {        // LDMDA/STMDA
            reg Rn(get_bits(instr_p.rest << 12, 4));
            bitset<16> reg_list(get_bits(instr_p.rest << 16, 16)); // list of registers
            int count = count_reg(reg_list); // number of set bits in `reg_list`

            // starting address
            word start_addr = ask_reg_get(bank, btoi(Rn)) - (count * 4) + 4;
            // used to check if the end_addr == the address - 4
            word end_addr = ask_reg_get(bank, btoi(Rn));
            // address to load into
            word address = start_addr;

            // check every set bit and update the register accordingly
            for (int i = 0; i < 15; ++i) {
                if (reg_list[i] == 1) {
                    if (instr_p.l_bit == 1) {
                        ask_reg_set(bank, i, get_host().mem_load(address));
                        address += 4; // increment `address` too next address
                        inc('l');
                    } else {
                        get_host().mem_store(address, ask_reg_get(bank, i));
                        address += 4; // increment `address` too next address
                        inc('s');
                    }
                }
            } 

            // if W bit is set, update Rn
            if (instr_p.w_bit == 1) {
                word val = ask_reg_get(bank, btoi(Rn)) - (count * 4);
                ask_reg_set(bank, btoi(Rn), val);
            }

        } else if (instr_p.p_bit == 0 && instr_p.u_bit == 1) { // LDMIA/STMIA / pop LDMFD
            reg Rn(get_bits(instr_p.rest << 12, 4));
            bitset<16> reg_list(get_bits(instr_p.rest << 16, 16)); // list of registers
            int count = count_reg(reg_list); // number of set bits in `reg_list`

            // starting address
            word start_addr = ask_reg_get(bank, btoi(Rn));
            // used to check if the end_addr == the address - 4
            word end_addr = ask_reg_get(bank, btoi(Rn)) + (count * 4) - 4;
            // address to load into
            word address = start_addr;

            // check every set bit and update the register accordingly
            for (int i = 0; i < 15; ++i) {
                if (reg_list[i] == 1) {
                    if (instr_p.l_bit == 1) {
                        ask_reg_set(bank, i, get_host().mem_load(address));
                        address += 4; // increment `address` too next address
                        inc('l');
                    } else {
                        get_host().mem_store(address, ask_reg_get(bank, i));
                        address += 4; // increment `address` too next address
                        inc('s');
                    }
                }
            } 

            if (reg_list[15] == 1) {
                // TODO: idk
            }

            // if W bit is set, update Rn
            if (instr_p.w_bit == 1) {
                word val = ask_reg_get(bank, btoi(Rn)) + (count * 4);
                ask_reg_set(bank, btoi(Rn), val);
            }

        } else if (instr_p.p_bit == 1 && instr_p.u_bit == 0) { // LDMDB/STMDB / push STMFD
            reg Rn(get_bits(instr_p.rest << 12, 4));
            bitset<16> reg_list(get_bits(instr_p.rest << 16, 16)); // list of registers
            int count = count_reg(reg_list); // number of set bits in `reg_list`

            // starting address
            word start_addr = ask_reg_get(bank, btoi(Rn)) - (count * 4);
            // check every set bit and update the register accordingly
            word end_addr = ask_reg_get(bank, btoi(Rn)) - 4;
            // address to store
            word address = start_addr;

            // check every set bit and update RAM accordingly
            for (int i = 0; i < 15; ++i) {
                    
                if (reg_list[i] == 1) {
                    if (instr_p.l_bit == 1) {
                        ask_reg_set(bank, i, get_host().mem_load(address));
                        address += 4; // increment `address` too next address
                        inc('l');
                    } else {
                        get_host().mem_store(address, ask_reg_get(bank, i));
                        address += 4; // increment `address` too next address
                        inc('s');
                    }
                }
            }

            // if W bit is set, update Rn
            if (instr_p.w_bit == 1) {
                word val = ask_reg_get(bank, btoi(Rn)) - (count * 4);
                ask_reg_set(bank, btoi(Rn), val);
            }

        } else if (instr_p.p_bit == 1 && instr_p.u_bit == 1) { // LDMIB/STMIB  
            reg Rn(get_bits(instr_p.rest << 12, 4));
            bitset<16> reg_list(get_bits(instr_p.rest << 16, 16)); // list of registers
            int count = count_reg(reg_list); // number of set bits in `reg_list`

            // starting address
            word start_addr = ask_reg_get(bank, btoi(Rn)) + 4;
            // used to check if the end_addr == the address - 4
            word end_addr = ask_reg_get(bank, btoi(Rn)) + (count * 4);
            // address to load into
            word address = start_addr;

            // check every set bit and update the register accordingly
            for (int i = 0; i < 15; ++i) {
                if (reg_list[i] == 1) {
                    if (instr_p.l_bit == 1) {
                        ask_reg_set(bank, i, get_host().mem_load(address));
                        address += 4; // increment `address` too next address
                        inc('l');
                    } else {
                        get_host().mem_store(address, ask_reg_get(bank, i));
                        address += 4; // increment `address` too next address
                        inc('s');
                    }
                }
            } 

            // if W bit is set, update Rn
            if (instr_p.w_bit == 1) {
                word val = ask_reg_get(bank, btoi(Rn)) + (count * 4);
                ask_reg_set(bank, btoi(Rn), val);
            }
        }
    } else if (strncmp(instr, "swi", 3) == 0) {     // SWI
        // if (get_bits(int_instr << 8, 24) == 0x0) {
        isRunning = false;
        // } else {                
        //     word LR = ask_reg_get(bank, 14);
        //     bank = am_svc;
        //     ask_reg_set(bank, 14, LR);

        //     word cpsr = ask_cpsr_get();

        //     ask_spsr_set(bank, cpsr);

        //     cpsr &= 0xfffffff3;
        //     ask_cpsr_set(cpsr);


        //     ask_reg_set(bank, 15, 0x123c);
        // }

    } else { /* not implemented stuff & "non" instructions */ }

    return btype;
}
