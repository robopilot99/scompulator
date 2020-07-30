#include "scompulator.h"

Scompulator::Scompulator(std::ifstream &infile){
    std::string line;
    // 0 = Header
    // 1 = Body
    // 2 = Footer
    char section = 0;
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

void Scompulator::run(){

}

bool Scompulator::execute(){
    unsigned short instruction = memory[PC].data;
    Opcode opcode = (Opcode)((instruction & opcodeBitmask) >> 10);
    unsigned short operand = instruction & operandBitmask;
    bool halted = false;

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
        // some extra logic here to perform the shift
        AC = (operand >> 9)? AC >> -((int)(operand | opcodeBitmask)): AC << operand;
        break;
    case Opcode::ADDI:
        // Sign-extend the immediate if necessary
        AC += (operand >> 9)? (operand | opcodeBitmask): operand;
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
    default:
        if(opcode != Opcode::NOP){
            std::cout << "Unrecognized opcode " << std::hex << (int)opcode << std::endl;
        }
        break;
    }

    PC++;

    return halted;
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
        std::cout << "\033[1m";
    std::cout
    << "\033[93m" << std::hex << line << " : " // Line number
    << "\033[37m" << std::setw(5) << memory[line].data << " " // Value
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

unsigned int Scompulator::getMemSize(){
    return memSize;
}