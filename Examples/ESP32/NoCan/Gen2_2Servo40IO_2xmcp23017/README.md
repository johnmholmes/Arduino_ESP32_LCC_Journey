# ESP32 2-Servo 24-IO Example.

This sketch implements:
* two servos, each with three positions
   Positions cabe set to angles 0-180
   The 0 and 180 end-points of the servos can be configured.
* N i/o channels, each of which can be an input or an output,
   If an output it may be solid, pulse or flashing, and consumes an on- and 0ff-event. 
   If an input it produces an on- and off-event, which may each be delayed.  The 
     inputs can be with or with pullups, inverted or not, or can be toggle. 
* Has a variable number of 'native' i/o depending on the processor, see board.h
* Has two MCP23017, each with two banks of 8 i/o

It demonstrates: 
* CDI
* memstruct of EEPROM reflecting the CDI structure
* setting flags to refect whether eveentids are used as consumers, producers, or both, see: **const EIDTab eidtab[NUM_EVENT] PROGMEM**
* Initialization routine to initialize the EEPROM, see: **userInitAll()**
* Eventid processing to set a servo's position, see: **pceCallback(unsigned int index)**
* Sampling of inputs and producing events.
* MCP23017 via I2C.  Note channel 7 in each port can only be outputs, otherwise the I2C locks up. 

This sketch is configured to use the ACAN_ESP32Can.h library that has been changed by David Harris.

This sketch is tested on the DOIT ESP32 DEVKIT V1 and the sn65hvd230 transceiver module.  

The Servos are connected to pin 25 & 33.
The 8 native I/O are on pins 14,27,26,32,15,4,16,23 in this order when configuring.
The 16 MCP23017 pins are split into 2 banks of 8 A0 to A7 and B0 to B7
The MCP23017 uses the address of 0x27 Pin 21 SDA  Pin 22 SCL.

We now use a config.h tab to change the node ID which is in line 18 of the config.h The factory reset is changed in line 22.

Notes on MCP23017 use:
* - Avoid Pin 7 as Input: Move any input devices (switches, sensors) on GPA7/GPB7 to other available pins (0–6).
* - Use Hardware Reset: Connect the Reset pin (Pin 18) to the microcontroller and actively pull it high, or use 
     a 10k resistor to pull it up to VCC to prevent floating.
* - Add Decoupling Capacitors: Place a 0.1µF ceramic capacitor between VCC and GND as close to the chip as 
     possible to filter power noise, which can cause erratic behavior.
* - Use Pull-up Resistors on I2C: Ensure the SDA and SCL lines have pull-up resistors (typically 4.7kΩ).



