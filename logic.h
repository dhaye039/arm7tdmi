/* 
 * Header file for decoding and executing logic
 * Design Phase 3a
 */

#ifndef LOGIC_H
#define LOGIC_H

#include <bitset>
#include <vector>

using namespace std;

typedef bitset<1>   bit;            // used for 1 bit values
typedef bitset<3>   id;             // the 3 bit identifying part of the instruction right after the cond
typedef bitset<2>   shift;          // shift operation
typedef bitset<4>   code;           // used for condition and opcode
typedef bitset<4>   reg;            // used for registers
typedef bitset<5>   shift_val;      // shift immediate
typedef bitset<8>   bbyte;           // byte
typedef bitset<12>  shifter_op;     // shifter operand
typedef bitset<32>  b_word;         // binary word

/* The different parts of the instruction
 * TODO: figure out a better way to differentiate instructions 
 * using these (better variable names, different groupings, etc)
 */ 
typedef struct instr_parts {
    code cond;    // Condition flag
    id ident;     // The 3 bits that identify the instruction type/group
    
    // for data processing
    bit i_bit;    // I bit
    code opcode;  // The code that differentiates the different instructions
    bit s_bit;    // S bit

    // for other
    bit p_bit;    // P bit
    bit u_bit;    // U bit
    bit b_bit;    // B bit (also the S bit for ldm/stm)
    bit w_bit;    // W bit
    bit l_bit;    // L bit

    b_word rest;    // the rest of the instruction (usually the registers and an immediate/shifter value)
} instr_parts_t;

// Fetches a word from the address stored in pc (r15)
word        fetch(void);

// Get/set for `isRunning` variable
bool        get_running(void);
void        set_running(bool new_isRunning);

// Gets the host to call `ask_host_services_t` functions
ask_host_services_t get_host(void);

void        set_host(const ask_host_services *host);

// Convert string instruction to binary using bitset
b_word      convert_instr(word instr);

// Copies `num_bits` number of bits from the beginning of `bin_instr` and returns the decimal representation of it
word        get_bits(b_word bin_instr, int num_bits);

// Initialize struct with the different parts of the instruction
void        decode_init(b_word b_instr, struct instr_parts *);

// Decodes the instruction based on the variable in the instr_parts_t struct
char*       decode(instr_parts_t *);

// Executes the instruction given to it in the `instr_parts` struct 
vector<int> execute(word int_instr);

/* Increases loads or stores variable by 1 */
void 	    inc(char c);

/* Gets the current bank mode */
ask_mode_t  ask_bank_get();

/* gets/sets the specified spsr */
word        ask_spsr_get(ask_mode_t bank);
void        ask_spsr_set(ask_mode_t bank, int index);


#endif