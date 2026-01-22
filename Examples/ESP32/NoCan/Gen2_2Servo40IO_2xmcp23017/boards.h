
// Board definitions
// Add a new variant by copying the appropriate section and changing the pin definitions, etc. 

//#include "Config.h"

#if defined(NANO_BOARD)
  #ifndef ARDUINO_ARCH_AVR
    #error Choose the Nano
  #endif
  #define BOARD "Nano"
  //#define NUM_SERVOS 2
  #define SERVOPINS     A4,A5
  #define NUM_NATIVE_IO 11
  #define IOPINS        13,4,5,6,7,8,9,A0,A1,A2,A3
  #define NUM_IO        43 // calc by hand 11+32
  #ifndef USEGCSERIAL
    #define MCP2515_CS  = 10 ; 
    #define MCP2515_INT =  3 ;
    #define CAN_TX_PIN 99
    #define CAN_RX_PIN 99
    #include <ACAN2515.h>
  #endif // USEGCSERIAL
  #include <Servo.h>
  //#define EEPROMbegin EEPROM.begin(1000);
  #define EEPROMbegin 
  #define EEPROMcommit 
  #define WIRE_begin 


#elif defined(ESP32_BOARD)
  #define BOARD "ESP32"
  //#define NUM_SERVOS 2
  #define SERVOPINS     25, 33
  #define NUM_NATIVE_IO  8
  #define IOPINS        14,27,26,32,15,4,16,23
  #define NUM_IO        40 // calc by hand 8+32
  #define CAN_TX_PIN (gpio_num_t) 18
  #define CAN_RX_PIN (gpio_num_t) 19
  #ifndef USEGCSERIAL
    #include "ACAN_ESP32Can.h"
  #endif // USEGCSERIAL
  #include <ESP32Servo.h>
  #define EEPROMbegin if(!EEPROM.begin(1000)) { Serial.println("Failed to initialize EEPROM"); }
  #define EEPROMcommit EEPROM.commit();
  #define WIRE_begin Wire.begin(21, 22, 100000)


#elif defined(ATOM_BOARD)
  #define BOARD "Atom"
  //#define NUM_SERVOS 2
  #define SERVOPINS     25,21
  #define NUM_NATIVE_IO 2
  #define IOPINS        22,19
  #define NUM_IO        34 // calc by hand 2+32
  #define CAN_TX_PIN    (gpio_num_t)26
  #define CAN_RX_PIN    (gpio_num_t)32
  #ifndef USEGCSERIAL
    #include "ACAN_ESP32Can.h"
  #endif // USEGCSERIAL
  #include <ESP32Servo.h>
  #define EEPROMbegin EEPROM.begin(1000);
  //#define EEPROMbegin { dP("\n size of memstruct="); dP(sizeof(MemStruct)); if(!EEPROM.begin(sizeof(MemStruct)+100)) { Serial.println("Failed to initialize EEPROM"); }
  #define EEPROMcommit { EEPROM.commit(); dP("EEPROM COMMIT"); }
  #define WIRE_begin Wire.begin(23, 33, 100000)  // choose pins for I2C


#elif defined(MINIMA_BOARD)
  #define BOARD "Minima"
  //#define NUM_SERVOS 2
  #define SERVOPINS     2,3
  #define NUM_NATIVE_IO 8
  #define IOPINS        6,7,8,9,10,11,12,13
  #define NUM_IO        40 // calc by hand 8+32
  #define CAN_TX_PIN 4
  #define CAN_RX_PIN 5
  #ifndef USEGCSERIAL
    #include "R4.h"
  #endif // USEGCSERIAL
  #include <Servo.h>
  #define EEPROMbegin 
  #define EEPROMcommit 
  #define WIRE_begin 


#else
  #error "No board selected"

#endif

