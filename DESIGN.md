# Design Phase

### Types

- An instruction struct

    - Will contain different parts of the instructions after being "cooked"
    - Right now I have it set up so that the struct is created in the execute file and is initialized and passed through the decoding parts, then the initialized struct is used again for the executing parts. I may change this later if I run into problems down the line.

- bitset (for bit manipulation)

    - [bitset example/application](https://www.geeksforgeeks.org/cpp-bitset-and-its-application/#)

- The other structs inside `askapi.h`

### Functions

- `convert_instr:`
    - Converts string instruction to binary using bitset
    - Returns a `bitset<32>`

- `get_bits:`
    - Copies `num_bits` number of bits from the beginning of `bin_instr` and returns the string of it
    - Returns a `string`

- `decode_init:`
    - Initialize struct with the different parts of the instruction
    - Returns `void`

- `decode:`
    - Decodes the instruction based on the variable in the instr_parts_t struct
    - Returns a `string`
 
- `execute:`
    - Executes the instruction given to it in the `instr_parts` struct
    - Returns an `int` 
        - Not used

- `fetch:`
    - Fetches the "raw" instruction from RAM and passes it to the decoding logic
    - returns a `string`

- The other functions in `ask.cpp`

### Files

- `ask.cpp`: CPU state

- `askapi.h`: The header file with all of the CPU state structs/functions/etc.

- `decode.cpp`: Decoding logic

- `execute.cpp`: Execution logic

- `fetch.cpp`: fetching logic 

    - May incoorperate this into another file if it doesn't take too many extra functions

- `logic.h` 

    - The name is still a work in progress, but basically this will house all the functions for the fetch, decode, execute cycle.