#include "scompulator.h"

Scompulator::Scompulator(std::ifstream &infile){
    std::string line;
    // 0 = Header
    // 1 = Body
    // 2 = Footer
    char section = 0;
    // Loop through every line using a simple state machine to keep track of whether within the header, body or footer
    while ( getline (infile,line) )
    {
    //   std::cout << line << std::endl;
      if(section == 0){
        // Search for specific fields in the header in order to correctly determine memory properties
        int memDepth = 0;
        int memWidth = 0;
        char radix[4];
        if(sscanf(line.c_str(), "DEPTH = %d", &memDepth) == 1){
            memSize = memDepth;
        }
        if(sscanf(line.c_str(), "WIDTH = %d", &memWidth) == 1){
            if(memWidth != 16){
            std::cout << "Only MIF files with width 16 are supported" << std::endl;
            return;
            }
        }
        if(sscanf(line.c_str(), "ADDRESS_RADIX = %3s", radix) == 1){
            if(strcmp(radix, "HEX") != 0){
            std::cout << "Only MIF files with radix HEX are supported" << std::endl;
            return;
            }
        }
        if(sscanf(line.c_str(), "DATA_RADIX = %3s", radix) == 1){
            if(strcmp(radix, "HEX") != 0){
            std::cout << "Only MIF files with radix HEX are supported" << std::endl;
            return;
            }
        }
        if(line.find("BEGIN") != std::string::npos){
            if(memSize < 1){
                std::cout << "Warning: memory depth was not specified, setting to 2048 words" << std::endl;
                memSize = 2048;
            }
            // Allocate our memory block
            memory = new MemLocation[memSize];
            // Move to next section
            section = 1;
        }
          
        } else if (section == 1){
            // Parse the code body to set memory values
            unsigned int commentIndex;
            unsigned int address;
            unsigned int value;
            if(sscanf(line.c_str(), "%x : %x;%n", &address, &value, &commentIndex ) >= 2){
                // Matched format specifier
                // std::cout << address << ", " << value << ", " << commentIndex << ", " << std::endl;
                if(address >= memSize){
                    std::cout << "Warning: value " << value << " tried to be placed at " << address << ", which is outside memory" << std::endl;
                } else {
                    memory[address].data = value;
                    memory[address].comment = std::string(line.substr(commentIndex, std::string::npos));
                    if((value & opcodeBitmask) == 0 && (value & operandBitmask) != 0){
                        // If a memory location has no valid opcode value, but some
                        // operand value, it is probably in use as a constant
                        memory[address].dataType = MemType::DATA;
                    } else {
                        memory[address].dataType = MemType::UNKNOWN;
                    }

                    // An exclamation mark contained within the comment field of the MIF file will place a breakpoint when
                    // that location in memory is used as an instruction or operand
                    if(memory[address].comment.find("!") != std::string::npos){
                        memory[address].breakpoint = true;
                    } else {
                        memory[address].breakpoint = false;
                    }

                    if(memory[address].data != 0 && address > lastAddress){
                        lastAddress = address;
                    }
                }
            }
        } else if (section == 2){

        }
    }
}

Scompulator::~Scompulator(){
    delete[] memory;
    for(unsigned int i = 0; i < ioPorts.size(); i++){
        delete ioPorts[i];
    }
}

// Checks if the instruction at PC (or the next instruction to be executed if PC is not supplied)
// triggers a breakpoint
// 0 = no break
// 1 = break on instruction
// 2 = break on operand
char Scompulator::shouldBreak(unsigned short xPC){
    // First, the easy case, break on instruction
    if(memory[xPC].breakpoint){
        return 1;
    }
    // Now we should check if the relevant instruction even uses a memory location as an operand
    // It's faster to check against the list of instructions that do NOT use a memory location as the operand
    Opcode opcode = (Opcode)((memory[xPC].data & opcodeBitmask) >> 11);
    if(
        opcode == Opcode::NOP ||
        opcode == Opcode::SHIFT ||
        opcode == Opcode::ADDI ||
        opcode == Opcode::RETURN ||
        opcode == Opcode::IN ||
        opcode == Opcode::OUT ||
        opcode == Opcode::LOADI ||
        // Technically the jump location is a memory location, but this would cause
        // execution to halt twice, once for the jump and once for the actual instruction
        opcode == Opcode::JUMP || 
        opcode == Opcode::JPOS ||
        opcode == Opcode::JNEG ||
        opcode == Opcode::JZERO
    ){
        return 0;
    }
    // If we've reached this point then some memory location is in use as an operand. Need to check
    // if that memory location has a breakpoint set
    unsigned short operand = memory[xPC].data & operandBitmask;
    if(memory[operand].breakpoint){
        return 2;
    }
    // None of the conditions were met, no break
    return 0;
}

char Scompulator::shouldBreak(){
    return shouldBreak(this->PC);
}

bool Scompulator::execute(){
    u_int16_t instruction = memory[PC].data;
    Opcode opcode = (Opcode)((instruction & opcodeBitmask) >> 11);
    u_int16_t operand = instruction & operandBitmask;
    bool halted = false;

    // Perform checks to verify correct seperation of data/instructions
    if(memory[PC].dataType == MemType::UNKNOWN){
        memory[PC].dataType = MemType::INSTRUCTION;
    } else if(memory[PC].dataType == MemType::DATA) {
        std::cout << "Warning: using " << std::hex << PC << " as instruction" << std::endl;
    }

    if(
        opcode == Opcode::LOAD ||
        opcode == Opcode::STORE ||
        opcode == Opcode::ADD ||
        opcode == Opcode::SUB ||
        opcode == Opcode::AND ||
        opcode == Opcode::OR ||
        opcode == Opcode::XOR
    ) {
        if(operand < memSize && memory[operand].dataType == MemType::UNKNOWN){
            memory[operand].dataType = MemType::DATA;
        } else if(operand < memSize && memory[operand].dataType == MemType::INSTRUCTION){
            std::cout << "Warning: using " << std::hex << PC << " as data" << std::endl;
        }
    }

    switch (opcode)
    {
    case Opcode::LOAD:
        AC = memory[operand].data;
        break;
    case Opcode::STORE:
        if(operand < memSize){
            memory[operand].data = AC;
            if(operand > lastAddress)
                lastAddress = operand;
        } else {
            std::cout << "Warning: write outside bounds of memory" << std::endl;
        }
        break;
    case Opcode::ADD:
        AC += memory[operand].data;
        break;
    case Opcode::SUB:
        AC -= memory[operand].data;
        break;
    case Opcode::JUMP:
        if(operand == PC) halted = true; // In a jump that jumps to itself in a endless loop
        PC = operand;
        PC--; // We always add 1 to the PC later in this block
        break;
    case Opcode::JNEG:
        if(AC < 0) PC = operand - 1;
        break;
    case Opcode::JPOS:
        if(AC > 0) PC = operand - 1;
        break;
    case Opcode::JZERO:
        if(AC == 0) PC = operand - 1;
        break;
    case Opcode::AND:
        AC = AC & memory[operand].data;
        break;
    case Opcode::OR:
        AC = AC | memory[operand].data;
        break;
    case Opcode::XOR:
        AC = AC ^ memory[operand].data;
        break;
    case Opcode::SHIFT:
        // Shifting by a negative value is undefined behavior in C so we have to do
        // some extra logic here to perform the shift. The shift ammount is given in
        // sign-magnitude format
        AC = (operand >> 4)? AC >> (operand & shiftAmntBitmask): AC << (operand & shiftAmntBitmask);
        break;
    case Opcode::ADDI:
        // Sign-extend the immediate if necessary
        AC += (operand >> 10)? (operand | opcodeBitmask): operand;
        break;
    case Opcode::ILOAD:
        AC = memory[memory[operand].data].data;
        break;
    case Opcode::ISTORE:
        if(memory[operand].data < memSize){
            memory[memory[operand].data].data = AC;
            if(memory[operand].data > lastAddress)
                lastAddress = memory[operand].data;
        } else {
            std::cout << "Warning: write outside bounds of memory" << std::endl;
        }
        break;
    case Opcode::CALL:
        if(PC_StackPtr > 9){
            std::cout << "Warning: stack depth greater then 10, behavior undefined, ignoring subroutine call" << std::endl;
        } else {
            PC_Stack[PC_StackPtr] = PC;
            PC_StackPtr++;
            PC = operand - 1;
        }
        break;
    case Opcode::RETURN:
        if(PC_StackPtr < 1){
            std::cout << "Warning: returning while not in subroutine, behavior undefined, terminating execution" << std::endl;
            halted = true;
        } else {
            PC_StackPtr--;
            PC = PC_Stack[PC_StackPtr];
        }
        break;
    case Opcode::IN:
        processIn(operand);
        break;
    case Opcode::OUT:
        processOut(operand);
        break;
    case Opcode::LOADI:
        // Sign-extend the immediate if necessary
        AC = (operand >> 10)? (operand | opcodeBitmask): operand;
        break;
    default:
        if(opcode != Opcode::NOP){
            std::cout << "Warning: unrecognized opcode " << std::hex << (int)opcode << std::endl;
        }
        break;
    }

    PC++;
    if(halted){
        this->halted = true;
    }
    return halted;
}

bool Scompulator::toggleBreak(uint16_t location){
    if(memory[location].breakpoint){
        memory[location].breakpoint = false;
        std::cout << "Breakpoint removed at [" << location << "]" << std::endl;
    } else {
        memory[location].breakpoint = true;
        std::cout << "Breakpoint added at [" << location << "]" << std::endl;
    }
    return true; // Assume this always succeeds
}

void Scompulator::processIn(unsigned char address){
    bool validPort = false;
    for(unsigned int i = 0; i < ioPorts.size(); i++){
        if(ioPorts[i]->address == address){
            AC = ioPorts[i]->in();
            validPort = true;
            break;
        }
    }
    if(validPort == false){
        std::cout << "Warning: read from invalid IO port " << address;
    }
}

void Scompulator::processOut(unsigned char address){
    bool validPort = false;
    for(unsigned int i = 0; i < ioPorts.size(); i++){
        if(ioPorts[i]->address == address){
            ioPorts[i]->out(AC);
            validPort = true;
            break;
        }
    }
    if(validPort == false){
        std::cout << "Warning: write to invalid IO port " << address;
    }
}

void Scompulator::dumpLine(unsigned int line){
    if(line == PC)
        std::cout << "\033[1m"; // Bold next instruction

    // Color code the line number in accordance with the memory type
    if(memory[line].dataType == MemType::INSTRUCTION){
        std::cout << "\033[93m";
    } else if (memory[line].dataType == MemType::DATA){
        std::cout << "\033[94m";
    } else {
        std::cout << "\033[90m";
    }

    std::cout
    << std::hex << std::setw(4) << line << " : " // Line number
    << "\033[37m" << std::setw(5) << memory[line].data << " "; // Value

    if(memory[line].breakpoint){
        std::cout << "\033[91m" << "! ";
    } else {
        std::cout << "  ";
    }

    std::cout
    << "\033[90m" << memory[line].comment 
    << "\033[0m"  << std::endl;
}

void Scompulator::dumpMemory(){
    std::cout << "AC: " << (signed short)AC << std::endl;
    std::cout << "PC: " << PC << std::endl;

    for(unsigned int i = 0; i <= lastAddress; i++){
        dumpLine(i);
    }
    if(lastAddress < memSize){
        std::cout << std::dec;
        std::cout << "Omitted " << memSize - lastAddress << " memory locations with value 0" << std::endl;
    }
}

unsigned short Scompulator::getPC(){
    return PC;
}

uint16_t Scompulator::getAC(){
    return AC;
}

unsigned int Scompulator::getMemSize(){
    return memSize;
}

bool Scompulator::hasHalted(){
    return halted;
}