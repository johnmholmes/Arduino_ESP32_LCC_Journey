// Global defs
const bool USE_90_ON_STARTUP = true;  // move 


// Board definitions

#if defined(ESP32_BOARD)
  #define NUM_SERVO 2
  #define NUM_IO 8
  #define CAN_TX_PIN (gpio_num_t) 18
  #define CAN_RX_PIN (gpio_num_t) 19
  #include "ACAN_ESP32Can.h"
  #include <ESP32Servo.h>
  uint8_t servopin[]  = { 25, 33 };
  uint8_t iopin[] = { 14,27,26,32,15,4,16,23 };
  //#define EEPROMbegin EEPROM.begin(1000);
  #define EEPROMbegin if(!EEPROM.begin(sizeof(MemStruct)+100)) { Serial.println("Failed to initialize EEPROM"); }
  #define EEPROMcommit EEPROM.commit();

#elif defined(ATOM_BOARD)
  #define NUM_SERVO 2
  #define NUM_IO 4
  #define CAN_TX_PIN (gpio_num_t)26
  #define CAN_RX_PIN (gpio_num_t)32
  #include "ACAN_ESP32Can.h"
  #include <ESP32Servo.h>
  uint8_t servopin[]  = { 25, 21 };
  uint8_t iopin[] = { 22, 19, 23, 33};
  //#define EEPROMbegin EEPROM.begin(1000);
  #define EEPROMbegin if(!EEPROM.begin(sizeof(MemStruct)+100)) { Serial.println("Failed to initialize EEPROM"); }
  #define EEPROMcommit EEPROM.commit();

#elif defined(MINIMA_BOARD)
  #define NUM_SERVO 2
  #define NUM_IO 8
  #define CAN_TX_PIN 4
  #define CAN_RX_PIN 5
  #include "R4.h"
  #include <Servo.h>
  uint8_t servopin[]  = { 2, 3 };
  uint8_t iopin[] = { 6,7,8,9,10,11,12,13 };
  #define EEPROMbegin 
  #define EEPROMcommit 


#else
  #error "No board selected"

#endif
