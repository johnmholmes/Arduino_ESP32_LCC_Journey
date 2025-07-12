# ESP32 Examples

These sketches are written for the ESP32 while they may work on other MCUs. I am keeping them separated to make it easier for novice coders to work with them.

They demonstrates: 
* CDI
* memstruct of EEPROM reflecting the CDI structure
* setting flags to refect whether eveentids are used as consumers, producers, or both, see: **const EIDTab eidtab[NUM_EVENT] PROGMEM**
* Initialization routine to initialize the EEPROM, see: **userInitAll()**
* Eventid processing to set a servo's position, see: **pceCallback(unsigned int index)**
* Sampling of inputs and producing events.

## Bug report email

johnmarkholmes.jh@gmail.com
