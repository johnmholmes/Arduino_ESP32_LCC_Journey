#ifndef USER_CONFIG_H
#define USER_CONFIG_H

//************ USER DEFINITIONS ************************************

// Node ID --- this must come from a range controlled by the user.
// See: http://registry.openlcb.org/uniqueidranges
// To set a new NODEID, edit the line below.
#define NODE_ADDRESS 5,1,1,1,0x8E,0x20   // DIY range example, not for global use.

// Set to 1 to Force Reset to Factory Defaults, else 0.
// Must be done at least once for each new board.
#define RESET_TO_FACTORY_DEFAULTS 0

// Allow direct to JMRI via USB, without CAN controller, comment out for CAN
// (Note: disable debugging if this is chosen.)

// *********************************************************
//Do not alter below unless you know what you are doing!
//**********************************************************

#include "GCSerial.h"          //Only used for testing with JMRI or standalone nodes

// Debugging -- uncomment to activate debugging statements:
#define DEBUG Serial

#include <mdebugging.h>

// New ACan for MCP2515
#define ACAN_FREQ 8000000UL    // set for crystal freq feeding the MCP2515 chip
#define ACAN_CS_PIN 53         // set for the MCP2515 chip select pin (Mega SPI CS)
#define ACAN_INT_PIN 2         // set for the MCP2515 interrupt pin
#define ACAN_RX_NBUF 2         // number of receive buffers
#define ACAN_TX_NBUF 2         // number of transmit buffers
#if defined(ARDUINO_AVR_MEGA2560)
//#include "ACan.h"            // uses main library ACan class
#endif

// Board definitions
#define MANU "J Holmes"        // The manufacturer of node
#define MODEL "24in 24out"     // The model of the board
#define HWVERSION "0.1"        // Hardware version
#define SWVERSION "0.4"        // Software version,

// User defs
#define NUM_INPUT 24
#define NUM_CHANNEL 24         // number of pins
#define NUM_ACTION_SETS 10     // number of action sets
#define NUM_APS 10             // number of actions per sets
#define NUM_ACTION 100         // number of events/actions

#define NUM_EVENT ((NUM_INPUT * 2) + NUM_ACTION)

#endif //USER_CONFIG_H