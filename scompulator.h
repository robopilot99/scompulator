#ifndef _SCOMPULATOR_
#define _SCOMPULATOR_

#include <fstream>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <cstring>
#include <array>

#include "names.h"
#include "io.h"

#include "IODevices/leds.h"

enum class Opcode { 
    NOP = 0x00,
    LOAD = 0x01,
    STORE = 0x02,
    ADD = 0x03,
    SUB = 0x04,
    JUMP = 0x05,
    JNEG = 0x06,
    JPOS = 0x07,
    JZERO = 0x08,
    AND = 0x09,
    OR = 0x0A,
    XOR = 0x0B,
    SHIFT = 0x0C,
    ADDI = 0x0D,
    ILOAD = 0x0E,
    ISTORE = 0x0F,
    CALL = 0x10,
    RETURN = 0x11,
    IN = 0x12,
    OUT = 0x13,
};

enum class IoAddresses {
    SWITCHES = 0x00, // slide switches
    LEDS =     0x01, // red LEDs
    TIMER =    0x02, // timer, usually running at 10 Hz
    XIO =      0x03, // pushbuttons and some misc. inputs
    SSEG1 =    0x04, // seven-segment display (4-digits only)
    SSEG2 =    0x05, // seven-segment display (4-digits only)
    LCD =      0x06, // primitive 4-digit LCD display
    XLEDS =    0x07, // Green LEDs (and Red LED16+17)
    BEEP =     0x0A, // Control the beep
    CTIMER =   0x0C, // Configurable timer for interrupts
    LPOS =     0x80, // left wheel encoder position (read only)
    LVEL =     0x82, // current left wheel velocity (read only)
    LVELCMD =  0x83, // left wheel velocity command (write only)
    RPOS =     0x88, // same values for right wheel...
    RVEL =     0x8A, // ...
    RVELCMD =  0x8B, // ...
    I2C_CMD =  0x90, // I2C module's CMD register,
    I2C_DATA = 0x91, // ... DATA register,
    I2C_RDY  = 0x92, // ... and BUSY register
    UART_DAT = 0x98, // UART data
    UART_RDY = 0x99, // UART status
    SONAR =    0xA0, // base address for more than 16 registers....
    DIST0 =    0xA8, // the eight sonar distance readings
    DIST1 =    0xA9, // ...
    DIST2 =    0xAA, // ...
    DIST3 =    0xAB, // ...
    DIST4 =    0xAC, // ...
    DIST5 =    0xAD, // ...
    DIST6 =    0xAE, // ...
    DIST7 =    0xAF, // ...
    SONALARM = 0xB0, // Write alarm distance; read alarm register
    SONARINT = 0xB1, // Write mask for sonar interrupts
    SONAREN =  0xB2, // register to control which sonars are enabled
    XPOS =     0xC0, // Current X-position (read only)
    YPOS =     0xC1, // Y-position
    THETA =    0xC2, // Current rotational position of robot (0-359)
    RESETPOS = 0xC3, // write anything here to reset odometry to 0
    RIN =      0xC8,
    LIN =      0xC9,
    IR_HI =    0xD0, // read the high word of the IR receiver (OUT will clear both words)
    IR_LO =    0xD1, // read the low word of the IR receiver (OUT will clear both words)
};

enum class MemType { 
    UNKNOWN,
    DATA,
    INSTRUCTION
};

struct MemLocation { // Struct for represnenting the contents of a single address in memory
    unsigned short data = 0; // 16 bit wide memory width
    std::string comment = ""; // Comment read in from MIF file
    // Store whether a particular memory address is in use as
    // data or an instruction in order to detect common errors
    MemType dataType = MemType::UNKNOWN; 
};

// struct IoDevice {
//     unsigned char address;
//     std::string name = "";
// };

// struct IoDevice IoDevices[] = {
//     {0x00, "SWITCHES"},
//     {0x01, "LEDS"}
// };

class Scompulator{
    private:
        std::ifstream infile;
        MemLocation* memory;
        unsigned int memSize;
        unsigned int lastAddress = 0; // Last address that has an actual value in it

        // The one and only accumulator register, 16 bits exactly for compatibility with SCOMP
        u_int16_t AC = 0;
        unsigned short PC = 0;
        unsigned int PC_Stack [10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        unsigned char PC_StackPtr = 0; // Points to the next open index in the PC stack
        void processOut(unsigned char address);
        void processIn(unsigned char address);
    public:
        Scompulator(std::ifstream &infile);
        ~Scompulator();
        void run();
        // Perform the next instruction
        // Returns true if the execution has halted for whatever reason (jump jumping to itself)
        bool execute(); 
        void dumpLine(unsigned int line);
        void dumpMemory();

        unsigned short getPC();
        unsigned int getMemSize();
        std::array<IODevice*, 41> ioPorts = {
            new IODevice("SWITCHES", 0x00), // slide switches
            new     LEDs("LEDS",     0x01), // red LEDs
            new IODevice("TIMER",    0x02), // timer, usually running at 10 Hz
            new IODevice("XIO",      0x03), // pushbuttons and some misc. inputs
            new IODevice("SSEG1",    0x04), // seven-segment display (4-digits only)
            new IODevice("SSEG2",    0x05), // seven-segment display (4-digits only)
            new IODevice("LCD",      0x06), // primitive 4-digit LCD display
            new IODevice("XLEDS",    0x07), // Green LEDs (and Red LED16+17)
            new IODevice("BEEP",     0x0A), // Control the beep
            new IODevice("CTIMER",   0x0C), // Configurable timer for interrupts
            new IODevice("LPOS",     0x80), // left wheel encoder position (read only)
            new IODevice("LVEL",     0x82), // current left wheel velocity (read only)
            new IODevice("LVELCMD",  0x83), // left wheel velocity command (write only)
            new IODevice("RPOS",     0x88), // same values for right wheel...
            new IODevice("RVEL",     0x8A), // ...
            new IODevice("RVELCMD",  0x8B), // ...
            new IODevice("I2C_CMD",  0x90), // I2C module's CMD register,
            new IODevice("I2C_DATA", 0x91), // ... DATA register,
            new IODevice("I2C_RDY",  0x92), // ... and BUSY register
            new IODevice("UART_DAT", 0x98), // UART data
            new IODevice("UART_RDY", 0x99), // UART status
            new IODevice("SONAR",    0xA0), // base address for more than 16 registers....
            new IODevice("DIST0",    0xA8), // the eight sonar distance readings
            new IODevice("DIST1",    0xA9), // ...
            new IODevice("DIST2",    0xAA), // ...
            new IODevice("DIST3",    0xAB), // ...
            new IODevice("DIST4",    0xAC), // ...
            new IODevice("DIST5",    0xAD), // ...
            new IODevice("DIST6",    0xAE), // ...
            new IODevice("DIST7",    0xAF), // ...
            new IODevice("SONALARM", 0xB0), // Write alarm distance; read alarm register
            new IODevice("SONARINT", 0xB1), // Write mask for sonar interrupts
            new IODevice("SONAREN",  0xB2), // register to control which sonars are enabled
            new IODevice("XPOS",     0xC0), // Current X-position (read only)
            new IODevice("YPOS",     0xC1), // Y-position
            new IODevice("THETA",    0xC2), // Current rotational position of robot (0-359)
            new IODevice("RESETPOS", 0xC3), // write anything here to reset odometry to 0
            new IODevice("RIN",      0xC8),
            new IODevice("LIN",      0xC9),
            new IODevice("IR_HI",    0xD0), // read the high word of the IR receiver (OUT will clear both words)
            new IODevice("IR_LO",    0xD1) // read the new IODevice("low word ", 0xIR) //ceiver (OUT will clear both words)
        };
};

#endif // !_SCOMPULATOR_