#define ESP32_BOARD

// Debugging -- uncomment to activate debugging statements:
//#define DEBUG Serial

#define CAN_RX_PIN (gpio_num_t) 4
#define CAN_TX_PIN (gpio_num_t) 5

// Allow direct to JMRI via USB, without CAN controller, comment out for CAN
//#define USEGCSERIAL

// To set a new nodeid edit the next line
#define NODE_ADDRESS  5,1,1,1,0x8E,0x19  // must be unique from an address space owned by you or DIY

// To Force Reset EEPROM to Factory Defaults set this value to 1, else 0.
// Need to do this at least once.
#define RESET_TO_FACTORY_DEFAULTS 1

#define EEPROMSIZE 4096
  
#define EEPROMbegin if(!EEPROM.begin(sizeof(MemStruct)+100)) { Serial.println("Failed to initialize EEPROM"); }
#define EEPROMcommit EEPROM.commit();