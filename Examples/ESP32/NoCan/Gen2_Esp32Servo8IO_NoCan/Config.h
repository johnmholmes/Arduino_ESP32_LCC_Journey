#ifndef CONFIG_H
#define CONFIG_H

// Choose a board, uncomment one line, see boards.h
#define ESP32_BOARD
//#define ATOM_BOARD
//#define MINIMA_BOARD

// Debugging -- uncomment to activate debugging statements:
#define DEBUG Serial

// Allow direct to JMRI via USB, without CAN controller, comment out for CAN
#define USEGCSERIAL

// To set a new nodeid edit the next line
#define NODE_ADDRESS  5,1,1,1,0x8E,0x03  // must be unique from an address space owned by you or DIY

// To Force Reset EEPROM to Factory Defaults set this value to 1, else 0.
// Need to do this at least once.
#define RESET_TO_FACTORY_DEFAULTS 1

#define NUM_POS 3  // this is fixed at 3 for now, but we can arrange for variable number if wanted.
#define NUM_EVENT (NUM_SERVOS * NUM_POS) + (NUM_IO*2)

// Board definitions
#define MANU "OpenLCB"           // The manufacturer of node
#define MODEL BOARD "2Servo8IO"  // The model of the board
#define HWVERSION "0.1"          // Hardware version
#define SWVERSION "0.1"          // Software version

#ifdef USEGCSERIAL
  #include "GCSerial.h"
  #undef DEBUG           // Cannot use DEBUG when using GCSerial
#endif

// Global defs
const bool USE_90_ON_STARTUP = true;  // move 

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
//#include "BBoards.h"

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
#define _IOEID(n) _IOEID_##n
#define IOEID(n) _IOEID(n)

#endif // CONFIG_H

