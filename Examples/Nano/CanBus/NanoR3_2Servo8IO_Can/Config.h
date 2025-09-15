#ifndef USER_CONFIG_H
#define USER_CONFIG_H

//************ USER DEFINITIONS ************************************

// Node ID --- this must come from a range controlled by the user.
// See: http://registry.openlcb.org/uniqueidranges
// To set a new NODEID, edit the line below.
#define NODE_ADDRESS 5,1,1,1,0x8E,0x04   // DIY range example, not for global use.

// Set to 1 to Force Reset to Factory Defaults, else 0.
// Must be done at least once for each new board.
#define RESET_TO_FACTORY_DEFAULTS 1

// If you want the servopositions save, every x ms, set SAVEPERIOD > 0. 
// 300000 would be every 30 minutes, the EEPROM should last > 5 years. 
// Note: a write will not be done when the servo position has not changed. 
#define SAVEPERIOD 600000

#endif //USER_CONFIG_H