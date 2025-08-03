
#ifndef CONFIG_H
#define CONFIG_H

// Choose a board, uncomment one line, see boards.h
//#define NANO_BOARD
#define ESP32_BOARD
//#define ATOM_BOARD
//#define MINIMA_BOARD

// Debugging -- uncomment to activate debugging statements:
//#define DEBUG Serial

// Allow direct to JMRI via USB, without CAN controller, comment out for CAN
//#define USEGCSERIAL

// To set a new nodeid edit the next line
#define NODE_ADDRESS  5,1,1,1,0x8E,0x03  // must be unique from an address space owned by you or DIY

// To Force Reset EEPROM to Factory Defaults set this value to 1, else 0.
// Need to do this at least once.
#define RESET_TO_FACTORY_DEFAULTS 0

#include "boards.h"

#define NUM_SERVOS 2
#define NUM_POS 3
//#define NUM_NATIVE_IO 8 see boards
#define MCP_ADDRESS 0x27  //// Choose address for mcp23017 board
#define NUM_MCP_GROUPS 2
#define NUM_MCP_IO_PER_GROUP 8
#define NUM_MCP_IO (NUM_MCP_GROUPS * NUM_MCP_IO_PER_GROUP)
#define NUM_EVENT ((NUM_SERVOS * NUM_POS) + ((NUM_NATIVE_IO + (NUM_MCP_GROUPS * NUM_MCP_IO_PER_GROUP))*2))

// add MCP23017
#include <MCP23017.h>
MCP23017 mcp = MCP23017(MCP_ADDRESS); 

// Global defs
const bool USE_90_ON_STARTUP = true;  // move 

// Board definitions
#define MANU "OpenLCB"           // The manufacturer of node
#define MODEL BOARD "2Servo24IO" // The model of the board
#define HWVERSION "0.1"          // Hardware version
#define SWVERSION "0.1"          // Software version

#ifdef USEGCSERIAL
  #include "GCSerial.h"
  #undef DEBUG           // Cannot use DEBUG when using GCSerial
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Definitions for EIDTab
// These allow automatic regostering of eventids
// If NUM_SERVOS>8 or NUM_IO>16 these will need extending
// Expands depending on NUM_SERVOS and NUM_IO
#define REG_SERVO_OUTPUT(s) CEID(servos[s].pos[0].eid), CEID(servos[s].pos[1].eid), CEID(servos[s].pos[2].eid)
#define REG_IO(i) PCEID(io[i].onEid), PCEID(io[i].offEid)

#define _SERVOEID_1 REG_SERVO_OUTPUT(0)
#define _SERVOEID_2 _SERVOEID_1, REG_SERVO_OUTPUT(1)
#define _SERVOEID_3 _SERVOEID_2, REG_SERVO_OUTPUT(2)
#define _SERVOEID_4 _SERVOEID_3, REG_SERVO_OUTPUT(3)
#define _SERVOEID_5 _SERVOEID_4, REG_SERVO_OUTPUT(4)
#define _SERVOEID_6 _SERVOEID_5, REG_SERVO_OUTPUT(5)
#define _SERVOEID_7 _SERVOEID_6, REG_SERVO_OUTPUT(6)
#define _SERVOEID_8 _SERVOEID_7, REG_SERVO_OUTPUT(7)
#define _SERVOEID(n) _SERVOEID_##n
#define SERVOEID(n) _SERVOEID(n)

#define _IOEID_1 REG_IO(0)
#define _IOEID_2 _IOEID_1, REG_IO(1)
#define _IOEID_3 _IOEID_2, REG_IO(2)
#define _IOEID_4 _IOEID_3, REG_IO(3)
#define _IOEID_5 _IOEID_4, REG_IO(4)
#define _IOEID_6 _IOEID_5, REG_IO(5)
#define _IOEID_7 _IOEID_6, REG_IO(6)
#define _IOEID_8 _IOEID_7, REG_IO(7)
#define _IOEID_9 _IOEID_8, REG_IO(8)
#define _IOEID_10 _IOEID_9, REG_IO(9)
#define _IOEID_11 _IOEID_10, REG_IO(10)
#define _IOEID_12 _IOEID_11, REG_IO(11)
#define _IOEID_13 _IOEID_12, REG_IO(12)
#define _IOEID_14 _IOEID_13, REG_IO(13)
#define _IOEID_15 _IOEID_14, REG_IO(14)
#define _IOEID_16 _IOEID_15, REG_IO(15)
#define _IOEID_17 _IOEID_16, REG_IO(16)
#define _IOEID_18 _IOEID_17, REG_IO(17)
#define _IOEID_19 _IOEID_18, REG_IO(18)
#define _IOEID_20 _IOEID_19, REG_IO(19)
#define _IOEID_21 _IOEID_20, REG_IO(20)
#define _IOEID_22 _IOEID_21, REG_IO(21)
#define _IOEID_23 _IOEID_22, REG_IO(22)
#define _IOEID_24 _IOEID_23, REG_IO(23)
#define _IOEID_25 _IOEID_24, REG_IO(24)
#define _IOEID_26 _IOEID_25, REG_IO(25)
#define _IOEID_27 _IOEID_26, REG_IO(26)
#define _IOEID_28 _IOEID_27, REG_IO(27)
#define _IOEID_29 _IOEID_28, REG_IO(28)
#define _IOEID_30 _IOEID_29, REG_IO(29)
#define _IOEID_31 _IOEID_30, REG_IO(30)
#define _IOEID_32 _IOEID_31, REG_IO(31)
#define _IOEID(n) _IOEID_##n
#define IOEID(n) _IOEID(n)

#endif // CONFIG_H

