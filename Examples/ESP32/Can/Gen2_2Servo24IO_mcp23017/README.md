# ESP32 2-Servo 8-IO Example.


This sketch implements:
* two servos, each with three positions
   Positions cabe set to angles 0-180
   The 0 and 180 end-points of the servos can be configured.
* N i/o channels, each of which can be an input or an output,
   If an output it may be solid, pulse or flashing, and consumes an on- and 0ff-event. 
   If an input it produces an on- and off-event, which may each be delayed.  The 
     inputs can be with or with pullups, inverted or not, or can be toggle. 

It demonstrates: 
* CDI
* memstruct of EEPROM reflecting the CDI structure
* setting flags to refect whether eveentids are used as consumers, producers, or both, see: **const EIDTab eidtab[NUM_EVENT] PROGMEM**
* Initialization routine to initialize the EEPROM, see: **userInitAll()**
* Eventid processing to set a servo's position, see: **pceCallback(unsigned int index)**
* Sampling of inputs and producing events.

This sketch is configured to use the ACAN_ESP32Can.h library that has been changed by David Harris.

This sketch is tested on the DOIT ESP32 DEVKIT V1 and the sn65hvd230 transceiver module.  




