# Arduino Nano Example

This sketch was written for a Nano or other ATMega328.  

This sketch implements:
* two servos, each with three positions
   Positions can be set to angles 0-180
   
* The i/o channels, each of which can be an input or an output,
   If an output it may be solid, pulse or flashing. 

It demonstrates: 
* CDI
* memstruct of EEPROM reflecting the CDI structure
* setting flags to refect whether eveentids are used as consumers, producers, or both, see: **const EIDTab eidtab[NUM_EVENT] PROGMEM**
* Initialization routine to initialize the EEPROM, see: **userInitAll()**
* Eventid processing to set a servo's position, see: **pceCallback(unsigned int index)**
* Sampling of inputs and producing events.

