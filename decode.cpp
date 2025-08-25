#include <iostream>
#include <string>
#include <cstring>

#include "askapi.h"
#include "logic.h"

using namespace std;

char str_instr[5];      // The type of instruction that is being processed (ex. "ldr", "movs", "add", etc.)
ask_host_services_t hs;

word fetch(void) { 
    ask_mode_t bank = ask_bank_get();

    word address = ask_reg_get(bank, 15) - 8;
    word instruction = hs.mem_load(address);

    return instruction;
}

ask_host_services_t get_host(void) {
    return hs;
}

void set_host(const ask_host_services *host) {
    hs = *host;
}

// Converts instruction to binary using bitset
b_word convert_instr(word instr) {
    b_word b_instr(instr);
    return b_instr;
}

// Copies `num_bits` number of bits from the beginning of `bin_instr` and returns the decimal representation of it
word get_bits(b_word bin_instr, int num_bits) {
    word bits = 0;
    
    for (int i = 0; i < num_bits; ++i) {
        bits |= (bin_instr[31 - i] << (num_bits - 1 - i));
    }
    return bits;
}

// Initialize struct with the different parts of the instruction
void decode_init(b_word b_instr, struct instr_parts *instr) {
    // First 4 bits
    instr->cond = code(get_bits(b_instr, 4));

    // The 3 bit code that distinguishes between data processing and (not data processing?)
    instr->ident = id(get_bits(b_instr << 4, 3));
    instr->i_bit = bit(get_bits(b_instr << 6, 1));
    
    // data processing
    if (instr->ident == 0b000 || instr->ident == 0b001) {
        // Identifying bits
        instr->opcode = code(get_bits(b_instr << 7, 4));
        instr->s_bit = bit(get_bits(b_instr << 11, 1));
    } else {
        // Identifying bits
        instr->p_bit = bit(get_bits(b_instr << 7, 1));
        instr->u_bit = bit(get_bits(b_instr << 8, 1));
        instr->b_bit = bit(get_bits(b_instr << 9, 1));
        instr->w_bit = bit(get_bits(b_instr << 10, 1));
        instr->l_bit = bit(get_bits(b_instr << 11, 1));
    }

    // Last 20 bits (technically it's the whole thing, but shifted 12 bits so the top 12 bits are 0 which has no real functional use but oh well)
    instr->rest = b_word(get_bits(b_instr << 12, 20));
}

void set_instr_s(const char *instruction, instr_parts_t *instr) {
    strncpy(str_instr, instruction, 4);
    if (instr->s_bit == 0b1) { // s_bit == 1
        strncpy(str_instr, instruction, 5);
        str_instr[3] = 's'; 
        str_instr[4] = '\0';
    }  
}

bool check_cond(code cond) {
    bool passed = false;

    // Get N, Z, C, and V flags
    word cpsr = ask_cpsr_get();
    int n = (cpsr >> 31) & 0b1;
    int z = (cpsr >> 30) & 0b1;
    int c = (cpsr >> 29) & 0b1;
    int v = (cpsr >> 28) & 0b1;
    
    /* AL */ if (cond == 0b1110) { passed = true; } 
    /* EQ */ else if (cond == 0b0000) { if (z) passed = true; } 
    /* NE */ else if (cond == 0b0001) { if (!z) passed = true; } 
 /* CS/HS */ else if (cond == 0b0010) { if (c) passed = true; } 
 /* CC/LO */ else if (cond == 0b0011) { if (!c) passed = true; } 
    /* MI */ else if (cond == 0b0100) { if (n) passed = true; } 
    /* PL */ else if (cond == 0b0101) { if (!n) passed = true; } 
    /* VS */ else if (cond == 0b0110) { if (v) passed = true; } 
    /* VC */ else if (cond == 0b0111) { if (!v) passed = true; } 
    /* HI */ else if (cond == 0b1000) { if (c & !z) passed = true; } 
    /* LS */ else if (cond == 0b1001) { if (!c | z) passed = true; } 
    /* GE */ else if (cond == 0b1010) { if (n == v) passed = true; } 
    /* LT */ else if (cond == 0b1011) { if (n != v) passed = true; } 
    /* GT */ else if (cond == 0b1100) { if (z == 0 && n == v) passed = true; } 
    /* LE */ else if (cond == 0b1101) { if (z == 1 || n != v) passed = true; } 
    /* NV */ else {} // idk if we do this

    return passed;
}

// Decodes the instruction based on the variable in the instr_parts_t struct
char* decode(instr_parts_t *instr) {
    // reset str_instr
    for (int i = 0; i < 5; ++i) { str_instr[i] = '\0'; }

    // instruction decoding logic
    if (check_cond(instr->cond)) { // 'always' cond
        if (instr->ident == 0b000 || instr->ident == 0b001) { // data processing instr
            if (instr->opcode == 0b0000) { // and or mul
                if (code(get_bits(instr->rest << 24, 4)) == 0b1001) {
                    // bits 7 and 4 are set => mul
                    set_instr_s("mul", instr);
                } else {
                    set_instr_s("and", instr);
                }
            } else if (instr->opcode == 0b0001) { // eor
                set_instr_s("eor", instr);
            } else if (instr->opcode == 0b0010) { // sub
                set_instr_s("sub", instr);
            } else if (instr->opcode == 0b0100) { // add
                set_instr_s("add", instr);
            } else if (instr->opcode == 0b0011) { // rsb
                set_instr_s("rsb", instr);
            } else if (instr->opcode == 0b1001) { // bx
                set_instr_s("bx", instr);
            } else if (instr->opcode == 0b1010) { // cmp
                set_instr_s("cmp", instr);
            } else if (instr->opcode == 0b1100) { // orr
                set_instr_s("orr", instr);
            } else if (instr->opcode == 0b1101) { // mov
                set_instr_s("mov", instr);
            } else if (instr->opcode == 0b1110) { // bic
                set_instr_s("bic", instr);
            } else if (instr->opcode == 0b1111) { // mvn
                set_instr_s("mvn", instr);
            } else { /* opcode not implemented yet */ }
        } else if (instr->ident == 0b011 || instr->ident == 0b010) { // ldr/str
            // L lit 
            if (instr->l_bit == 1) {    // ldr
                strncpy(str_instr, "ldr\0", 4);
            } else {                    // str
                strncpy(str_instr, "str\0", 4);
            }            
        } else if (instr->ident == 0b100) { // ldm/stm
            // L lit 
            if (instr->l_bit == 1) {    // ldm
                strncpy(str_instr, "ldm\0", 4);
            } else {                    // stm
                strncpy(str_instr, "stm\0", 4);
            }       
        } else if (instr->ident == 0b101) { // b/bl
            if (instr->p_bit == 1) 
                strncpy(str_instr, "bl\0", 3);
            else 
                strncpy(str_instr, "b\0", 2);
        } else if (instr->ident == 0b111 && instr->i_bit == 0b1) { // swi
            set_instr_s("swi", instr);
        } else { /* other implementations  */ }
    } else { /* skip instruction */ 
        set_instr_s("non", instr);
    }
    return str_instr;
}
