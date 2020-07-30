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
    // If no configuration is supplied, just print out the current configuration
    if(configuration == ""){
        std::cout << name << " has value: " << stored << std::endl;
        return 0;
    }
    char * p;
    unsigned int value = strtol(configuration.c_str(), &p, 0);
    if(configuration.c_str() == p){
        std::cout << "Invalid argument '" << configuration << "'" << std::endl;
        return 0;
    } 
    stored = value;
    std::cout << name << " value set" << std::endl;
    return 1;
}