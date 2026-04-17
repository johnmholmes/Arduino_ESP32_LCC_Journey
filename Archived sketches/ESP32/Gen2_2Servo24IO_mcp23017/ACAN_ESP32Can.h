//https://github.com/pierremolinaro/acan-esp32
#ifndef ARDUINO_ARCH_ESP32
  #error "Select an ESP32 board"
#endif

#ifndef NOCAN
#define NOCAN

#pragma message "Compiling ACAN_ESP32Can.h"

#include "OlcbCan.h"
#include "debugging.h"

#include <ACAN_ESP32.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <core_version.h> // For ARDUINO_ESP32_RELEASE

#define P(...) Serial.print(__VA_ARGS__)
#define PV(x) { P(" " #x "="); P(x); }
#define PVH(x) { P(" " #x "="); P(x, HEX); }

class OlcbCanClass : public OlcbCan  {
  public:
    virtual void init() {                    // initialization
      //P("\n ACAN_ESP32 init");
      ACAN_ESP32_Settings settings(125000L);
      settings.mRxPin = CAN_RX_PIN;
      settings.mTxPin = CAN_TX_PIN;
      settings.mRequestedCANMode = ACAN_ESP32_Settings::NormalMode;
      if( !ACAN_ESP32::can.begin(settings) ) return;
      //P("\n ACAN_ESP32 init'd");
    }
    virtual uint8_t avail() { return 1; }                // read rxbuffer available
    virtual uint8_t read() {                 // read a buffer
      CANMessage frame;
      if( !ACAN_ESP32::can.receive(frame) ) return 0;
      if(frame.rtr) return 0;
      if(!frame.ext) return 0;
      id = frame.id;
      length = frame.len;
      memcpy(data, frame.data, length);
      //P("\n ACAN_ESP32 read "); PVH(id); PV(length); PVH(data[0]);
      return 1;
    }
    virtual uint8_t txReady() { return 1; }             // write txbuffer available
    virtual uint8_t write(long timeout) {    // write, 0= immediately or fail; 0< if timeout occurs fail
      CANMessage frame;
      //P("\n ACAN_ESP32 write >>> "); PVH(id); PV(length); PVH(data[0]);
      frame.id = id;
      frame.ext = true;
      frame.rtr = false;
      frame.len = length;
      memcpy(frame.data, data, length);
      while(!ACAN_ESP32::can.tryToSend(frame));
      //P(" OK");
      delay(5);
      return 1;
    }
    virtual uint8_t write() {               // write(0), ie write immediately
        return write((long) 0);
    };
    bool active;                          // flag net activity
};

#endif
