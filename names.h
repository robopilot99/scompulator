#include <iostream>

const std::string opcodeNames[] = {
    "NOP",
    "LOAD",
    "STORE",
    "ADD",
    "SUB",
    "JUMP",
    "JNEG",
    "JPOS",
    "JZERO",
    "AND",
    "OR",
    "XOR",
    "SHIFT",
    "ADDI",
    "ILOAD",
    "ISTORE",
    "CALL",
    "RETURN",
    "IN",
    "OUT"
};

const uint16_t opcodeBitmask    = 0xF800;
const uint16_t operandBitmask   = 0x03FF;
const uint16_t shiftAmntBitmask = 0x000F;