#include "leds.h"

LEDs::LEDs(std::string name, unsigned char address) :
    IODevice(name, address)
{
}

void LEDs::out(short value){
    stored = value;
    std::cout << name << " outputs " << value << std::endl;
    for(int i = 0; i < 16; i++){
        if(value << i & 0x8000){
            std::cout << "\033[32m";
        } else {
            std::cout << "\033[90m";
        }
        std::cout << "*";
    }
    std::cout << "\033[0m" << std::endl;
}

short LEDs::in(){
    std::cout << "Warning: read from peripheral LEDs, which does not return any data" << std::endl;
    return 0;
}

unsigned int LEDs::config(std::string configuration){
    if(configuration == ""){
        for(int i = 0; i < 16; i++){
            if(stored << i & 0x8000){
                std::cout << "\033[32m";
            } else {
                std::cout << "\033[90m";
            }
            std::cout << "*";
        }
        std::cout << "\033[0m" << std::endl;
        return 0;
    }
    std::cout << "LEDs cannot be configured" << std::endl;
    return 1;
}