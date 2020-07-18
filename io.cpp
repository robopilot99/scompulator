#include "io.h"

IODevice::IODevice(std::string name, unsigned char address){
    this->name = name;
    this->address = address;
}

void IODevice::out(short value){
    std::cout << name << " outputs " << value << std::endl;
}

short IODevice::in(){
    std::cout << name << " reads " << stored << std::endl;
    return 0;
}

unsigned int IODevice::config(std::string configuration){
    try {
        stored = std::stoi(configuration);
        std::cout << name << " value set" << std::endl;
    } catch (const std::invalid_argument &){
        std::cout << "Invalid argument '" << configuration << "'" << std::endl;
    }
    return 1;
}