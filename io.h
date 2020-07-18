#ifndef _IODevice_
#define _IODevice_

#include <string>
#include <iostream>

class IODevice {
    public:
        IODevice(std::string name, unsigned char address);
        virtual void out(short value);
        virtual short in();
        // Function that can be used to support all kinds of config parameters
        virtual unsigned int config(std::string); 
        std::string name;
        unsigned char address; // Address on IO bus
    private:
        unsigned short stored = 0;
};

#endif // !_IODevice_