#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string>
#include <bitset>

#include "askapi.h"
#include "logic.h"

using namespace std;

int main() {
    ask_mode_t bank;

    execute(0xe3a00030); // mov	r0, #48	; 0x30
    assert(ask_reg_get(bank, 0) == 48);

    execute(0xe3a094ff); // mov	r9, #-16777216; 0xff000000
    assert(ask_reg_get(bank, 9) == 0xff000000);

    execute(0xe3a01f41); // mov r1, #260; 0x104
    assert(ask_reg_get(bank, 1) == 260);    

    execute(0xe1a01180); // lsl	r1, r0, #3
    assert(ask_reg_get(bank, 1) == 384);

    execute(0xe1a02081); // lsl	r2, r1, #1
    assert(ask_reg_get(bank, 2) == 768);

    execute(0xe1a08389); // lsl	r8, r9, #7
    assert(ask_reg_get(bank, 8) == 0x80000000);

    execute(0xe1a03120); // lsr	r3, r0, #2
    assert(ask_reg_get(bank, 3) == 12);

    execute(0xe1a041a1); // lsr	r4, r1, #3
    assert(ask_reg_get(bank, 4) == 48);

    execute(0xe1a07c29); // asr	r7, r0, #4
    assert(ask_reg_get(bank, 7) == 255);

    // TODO: Fix negatves
    // execute(0xe1a08c49); // asr	r8, r9, #24
    // assert(ask_reg_get(bank, 8) == );

    // execute(0xe1a06c29);

	cout << "ALL TESTS PASSED\n";
	return 0;
}