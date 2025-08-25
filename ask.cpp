/* 
 *  ASK Functions
 *  Author: David John Hayes 
 */

#include <stddef.h>
#include <iostream>
#include <cstring>
#include <vector>

/* ArmSim Kernel/Shell Interface definition */
#include "askapi.h"

/* Logic for fetching, decodeing, and executing */
#include "logic.h"

using namespace std;

// Global variables

sword registers[16];                        // Registers + 1
word cpsr = 0x11111;                        // SYS mode
word spsr_svc;

ask_config_t config_flags = ac_nothing;     // Configuration flags
unsigned int instr_cnt, loads, stores = 0;  // Instruction count, # of loads, # of stores

char** ask_info(void) {
    char** info = new char*[4];

    info[0] = new char[20];
    strcpy(info[0], "author=dhaye039\0");

    info[1] = new char[20];
    strcpy(info[1], "api=1.0\0");
    
    info[2] = new char[20];
    strcpy(info[2], "svc");

    info[3] = nullptr;


    return info;
}

void ask_init(const struct ask_host_services *host) {
    // Initialize registers to 0
    for (int i = 0; i < 21; i++) {
        registers[i] = 0;
    }

    // Clear configuration flags
    config_flags = ac_nothing;

    // Reset instruction counter
    instr_cnt, loads, stores = 0;

    // Initialize the host object with the provided services
    set_host(host);

    // Log the initialization message
    if (host) {
        host->log_msg("CPU initialized");
    }
}

void ask_config_set(ask_config_t flags) {
    config_flags = flags;
}

ask_config_t ask_config_get(void) {
    return config_flags;
}

void ask_stats_report(struct ask_stats *stats) {
    stats->instructions = instr_cnt;
    stats->loads = loads;
    stats->stores = stores;
}

word ask_reg_get(ask_mode_t bank, int index) {
    if (index == 15)
        return registers[index] + 8;
    else 
        return registers[index];

}

void ask_reg_set(ask_mode_t bank, int index, word value) {
    registers[index] = value;
}

word ask_cpsr_get(void) {
    return cpsr;
}

void ask_cpsr_set(word value) {
    cpsr = value;
}

word ask_spsr_get(ask_mode_t bank) {
    if (bank == am_svc) {
        return spsr_svc;
    } else {
        return 0;
    }
}

void ask_spsr_set(ask_mode_t bank, int index) {
    if (bank == am_svc) {
        spsr_svc = cpsr;
    }
}

int	ask_cpu_running(void) {
    return (instr_cnt == 0) ? 0 : 1;
}

void ask_cpu_signal(ask_signal_t signal) {
    switch (signal) {
        case as_halt: 
            set_running(false);
            break;
        case as_irq:
            //
            break;
        case as_fiq:
            //
            break;
        default:
            break;
    }

}

int	ask_cpu_run(int cycles) {
    set_running(true);
    while (get_running() && (cycles == 0 || instr_cnt < cycles)) {
        word instr = fetch(); ++loads;
        vector<int> b = execute(instr); ++instr_cnt;
        if (config_flags & ac_trace_log) {
            get_host().log_trace(
                instr_cnt, registers[15], cpsr, registers[0], registers[1], registers[2], 
                registers[3], registers[4], registers[5], registers[6], registers[7], registers[8], 
                registers[9], registers[10], registers[11], registers[12], registers[13], registers[14]
            );
            
          
        }
        if (b[0] == 1) {
            ask_reg_set(ask_bank_get(), 15, registers[15] + 8 + b[1]);
        }
        else if (b[0] == 2)
            ask_reg_set(ask_bank_get(), 15, b[1]);
        else
            ask_reg_set(ask_bank_get(), 15, registers[15] + 4);

    }
    
    return instr_cnt;
}

void inc(char c) {
    if (c == 'l') { ++loads; }
    else if (c == 's') { ++stores; } 
    else {}    
}

