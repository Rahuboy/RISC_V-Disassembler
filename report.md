# Report
The disassembler supports the following instructions:
``` add, sub, and, or, xor, sll, srl, sra, addi, andi, ori, xori, slli, srli, srai, ld, lw, lh, lb, lwu, lhu, lbu, sd, sw, sh, sb, beq, bne, blt, bge, bltu, bgeu, jal, jalr, lui ```

It also automatically labels the code whenever required.

### Code
- Global Variables:
    - A vector of the struct `instruction` was used to implement the disassembler. The fields of the struct are index, bstring (a binary string of the instruction), the instruction type (R-Format, S-Format etc.), and variables that help implement the labelling.
    - The global variable `label_number` helped with numbering the various labels
- Functions:
    - `instruction_type_finder` helps finding the format of the instructions, by matching the opcode to the corresponding instruction format. 
    - `imm_finder_b` helps calculate the immediate for 'B-Format' instructions
    - `imm_finder_j` helps calculate the immediate for 'J-Format' instructions
    - `labeller` takes care of the labelling. It uses immediate calculator functions to find the instruction to be labelled, and changes the appropriate fields in the global variables to alter the label. 
    - `parser` is responsible for printing the final output. It extracts fields from the binary instruction, and converts it to the assembly equivalent.

- Execution Flow:
    - First the lines of the file `input.txt` are read, and the members of the global vector are populated. The lines of hex instructions are converted into binary.
    - Then the `labeller` function is called on all members of the vector, and the appropriate label fields are modified.
    - Finally, the function `parser` is called on all the structs, and the final output is printed.

### Testing
- The code was tested individually on the various instructions previously described. The testing also took into consideration the ranges for the immediates and negative values. 
- The code was also tested against several example programs: 
For example, the list {0001b203, 0081b283, 01018193, 00000513, 00018633, 02a20263, 00000593, 00b28a63, 00063a03, 00860613, 00158593, fe0008e3, 00150513, fe0000e3, 00000013} (each on a different line in 'input.txt') was disassembled into 
```
ld x4, 0(x3)
ld x5, 8(x3)
addi x3, x3, 16
addi x10, x0, 0
add x12, x3, x0
L3: beq x4, x10, L0
addi x11, x0, 0
L2: beq x5, x11, L1
ld x20, 0(x12)
addi x12, x12, 8
addi x11, x11, 1
beq x0, x0, L2
L1: addi x10, x10, 1
beq x0, x0, L3
L0: addi x0, x0, 0
```