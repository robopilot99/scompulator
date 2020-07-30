#include "../io.h"

class LEDs: public IODevice {
    public:
        LEDs(std::string name, unsigned char address);
        void out (short value) override;
        short in () override;
        // Function that can be used to support all kinds of config parameters
        unsigned int config(std::string) override; 
};