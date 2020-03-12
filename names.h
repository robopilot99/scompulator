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

const short opcodeBitmask  = 0xFC00;
const short operandBitmask = 0x03Ff;