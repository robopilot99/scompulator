#include <fstream>
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include "scompulator.h"

// Splits a string be deliminator
std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start) {
       tokens.push_back(text.substr(start));
    }
    return tokens;
}

std::string toLower(std::string in){
    std::transform(in.begin(), in.end(), in.begin(), [](unsigned char c){ return std::tolower(c); }); // To lowercase
    return in;
}

// Returns a parsed int or -1 if value could not be parsed
unsigned int parseInt(std::string in){
   char * p;
   unsigned int value = strtol(in.c_str(), &p, 0);
   if(in.c_str() == p){
       return -1;
   } 
   return value;
}

int main(){
    std::ifstream infile;
    infile.open("./Test Programs/IO.mif");
    Scompulator scomp(infile);
    std::vector<std::string> inputs;
    std::string input;
    std::string command;
    std::string arg1;
    std::string arg2;

    while(true){
        input = "";
        command = "";
        scomp.dumpLine(scomp.getPC());
        std::cout << ">";
        getline(std::cin, input);
        input = toLower(input);
        inputs = split(input, 0x20);
        while(inputs.size() < 3){
            inputs.push_back(std::string(""));
        }
        command = inputs[0];
        if(command == "s" || command == "step" || command == ""){ // Step
            scomp.execute();
        } else if (command == "d" || command == "dump"){ // Dump
            scomp.dumpMemory();
        } else if (command == "c" || command == "context"){ // Context
            std::cout << "---------------------" << std::endl;
            unsigned int start = scomp.getPC();
            if(start > 4){
                start -= 4;
            } else {
                start = 0;
            }
            unsigned int end = scomp.getPC() + 4;
            if(end > scomp.getMemSize()) end = scomp.getMemSize();
            for(unsigned int i = start; i < end; i++){
                scomp.dumpLine(i);
            }
            std::cout << "---------------------" << std::endl;
        } else if (command == "io"){
            if(inputs[1] == ""){
                std::cout << "Usage: io device [value]" << std::endl;
            } else {
                int deviceIndex = -1;
                int inputAddress = parseInt(inputs[1]);
                if(inputAddress == -1){
                    // Value was not parsable as an int, check for a match by name
                    std::string thisName = inputs[1];
                    for(unsigned int i = 0; i < scomp.ioPorts.size(); i++){
                        std::string otherName = scomp.ioPorts[i]->name;
                        if(inputs[1] == toLower(otherName)){
                            deviceIndex = i;
                            break;
                        }
                    }
                } else {
                    // Value was parsable as an int, check for a match by address
                    for(unsigned int i = 0; i < scomp.ioPorts.size(); i++){
                        if(inputAddress == scomp.ioPorts[i]->address){
                            deviceIndex = i;
                            break;
                        }
                    }
                }
                if(deviceIndex == -1){
                    std::cout << "Invalid device '" << inputs[1] << "'" << std::endl;
                } else {
                    // We have index of a real IO device, call config function
                    scomp.ioPorts[deviceIndex]->config(inputs[2]);
                }
            }
        } else if (command == "exit"){
            break;
        } else if (command == "help"){
            std::cout <<
            "context" << std::endl <<
            "Display several lines of context before and after the PC. Aliased by 'c" << std::endl <<
            "dump" << std::endl <<
            "Displays the entire contents of SCOMP memory. Aliased by 'd'" << std::endl <<
            "exit" << std::endl <<
            "Terminates SCOMPULATOR" << std::endl <<
            "step" << std::endl <<
            "Simulate a single cycle of scomp operatoin. Aliased by 's'. Default action when nothing is entered in the command-prompt" << std::endl <<
            "help" << std::endl <<
            "Displays a list of all SCOMPULATOR commands. How you got here" << std::endl;

        } else {
            std::cout << command << " is not a valid command. try 'help'" << std::endl;
        }
    }
}