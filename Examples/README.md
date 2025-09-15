# Arduino Nano, Mega, and ESP32 Examples

These are sketches are written allocated by a certain type of Arduino Eco-system board.

They demonstrates: 
* CDI
* memstruct of EEPROM reflecting the CDI structure
* setting flags to refect whether eveentids are used as consumers, producers, or both, see: **const EIDTab eidtab[NUM_EVENT] PROGMEM**
* Initialization routine to initialize the EEPROM, see: **userInitAll()**
* Eventid processing to set a servo's position, see: **pceCallback(unsigned int index)**
* Sampling of inputs and producing events.

### Standalone or Canbus network
  - These sketches can be used in various ways as shown in the video series.

### Working with Servos
  - When working with servos they should really be power from a seperate 5 volt supply and the MCU on supplying the data for the servo movement.
  - Servo leads should be kept short to avoid picking up noise from othe per circuits. or try to cross these at 90 degrees, and running them away from other voltage sources.

