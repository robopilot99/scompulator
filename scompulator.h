#include <fstream>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <cstring>

#include "names.h"

enum class Opcode { 
    NOP = 0x00,
    LOAD = 0x01,
    STORE = 0x02,
    ADD = 0x03,
    SUB = 0x04,
    JUMP = 0x05,
    JNEG = 0x06,
    JPOS = 0x07,
    JZERO = 0x08,
    AND = 0x09,
    OR = 0x0A,
    XOR = 0x0B,
    SHIFT = 0x0C,
    ADDI = 0x0D,
    ILOAD = 0x0E,
    ISTORE = 0x0F,
    CALL = 0x10,
    RETURN = 0x11,
    IN = 0x12,
    OUT = 0x13,
};

enum class MemType { 
    UNKNOWN,
    DATA,
    INSTRUCTION
};

struct MemLocation { // Struct for represnenting the contents of a single address in memory
    unsigned short data = 0; // 16 bit wide memory width
    std::string comment = ""; // Comment read in from MIF file
    // Store whether a particular memory address is in use as
    // data or an instruction in order to detect common errors
    MemType dataType = MemType::UNKNOWN; 
};

class Scompulator{
    private:
        std::ifstream infile;
        MemLocation* memory;
        unsigned int memSize;

        // The one and only accumulator register, 16 bits exactly for compatibility with SCOMP
        u_int16_t AC = 0;
        short PC = 0;
    public:
        Scompulator(std::ifstream &infile);
        void run();
        // Perform the next instruction
        // Returns true if the execution has halted for whatever reason (jump jumping to itself)
        bool execute(); 
        void dumpMemory();
};