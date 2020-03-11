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
        if(sscanf(line.c_str(), "ADDRESS_RADIX = %3s", &radix) == 1){
            if(strcmp(radix, "HEX") != 0){
            std::cout << "Only MIF files with radix HEX are supported" << std::endl;
            return;
            }
        }
        if(sscanf(line.c_str(), "DATA_RADIX = %3s", &radix) == 1){
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
                }
            }
        } else if (section == 2){

        }
    }
}

void Scompulator::run(){

}

void Scompulator::execute(){

}

void Scompulator::dumpMemory(){
    for(int i = 0; i < memSize; i++){
        std::cout << std::hex << i << " : " << memory[i].data << " " << memory[i].comment << std::endl;
    }
}