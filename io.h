#ifndef _IODevice_
#define _IODevice_

#include <string>
#include <iostream>

class IODevice {
    public:
        IODevice(std::string name, unsigned char address);
        virtual ~IODevice() = default;
        virtual void out(short value);
        virtual short in();
        // Function that can be used to support all kinds of config parameters, may be passed an empty string
        virtual unsigned int config(std::string); 
        std::string name;
        unsigned char address; // Address on IO bus
    protected:
        unsigned short stored = 0;
};

#endif // !_IODevice_