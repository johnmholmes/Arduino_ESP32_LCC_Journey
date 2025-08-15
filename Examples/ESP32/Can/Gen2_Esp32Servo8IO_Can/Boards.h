
// Board definitions

#include "Config.h"

#if defined(ESP32_BOARD)
  #define BOARD "ESP32"
  #define NUM_SERVOS 2
  #define SERVOPINS 25, 33
  #define NUM_IO    8
  #define IOPINS    14,27,26,32,15,4,16,23
  #define CAN_TX_PIN (gpio_num_t) 18
  #define CAN_RX_PIN (gpio_num_t) 19
  #ifndef USEGCSERIAL
    #include "ACAN_ESP32Can.h"
  #endif // USEGCSERIAL
  #include <ESP32Servo.h>
  //#define EEPROMbegin EEPROM.begin(1000);
  #define EEPROMbegin if(!EEPROM.begin(sizeof(MemStruct)+100)) { Serial.println("Failed to initialize EEPROM"); }
  #define EEPROMcommit EEPROM.commit();

#elif defined(ATOM_BOARD)
  #define BOARD "Atom"
  #define NUM_SERVOS 2
  #define SERVOPINS 25, 21
  #define NUM_IO    4
  #define IOPINS    22, 19, 23, 33
  #define CAN_TX_PIN (gpio_num_t)26
  #define CAN_RX_PIN (gpio_num_t)32
  #ifndef USEGCSERIAL
    #include "ACAN_ESP32Can.h"
  #endif // USEGCSERIAL
  #include <ESP32Servo.h>
  #define EEPROMbegin EEPROM.begin(1000);
  //#define EEPROMbegin { dP("\n size of memstruct="); dP(sizeof(MemStruct)); if(!EEPROM.begin(sizeof(MemStruct)+100)) { Serial.println("Failed to initialize EEPROM"); }
  #define EEPROMcommit { EEPROM.commit(); dP("EEPROM COMMIT"); }


#elif defined(MINIMA_BOARD)
  #define BOARD "Minima"
  #define NUM_SERVOS 2
  #define SERVOPINS 2, 3
  #define NUM_IO    8
  #define IOPINS    6,7,8,9,10,11,12,13
  #define CAN_TX_PIN 4
  #define CAN_RX_PIN 5
  #ifndef USEGCSERIAL
    #include "R4.h"
  #endif // USEGCSERIAL
  #include <Servo.h>
  #define EEPROMbegin 
  #define EEPROMcommit 


#else
  #error "No board selected"

#endif
