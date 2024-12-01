# Arduino Nano Examples

These sketches are written for an Arduino Nano but may work for other MCUs but I have only tested them with the Nano.

It demonstrates: 
* CDI
* memstruct of EEPROM reflecting the CDI structure
* setting flags to refect whether eveentids are used as consumers, producers, or both, see: **const EIDTab eidtab[NUM_EVENT] PROGMEM**
* Initialization routine to initialize the EEPROM, see: **userInitAll()**
* Eventid processing to set a servo's position, see: **pceCallback(unsigned int index)**
* Sampling of inputs and producing events.

### Standalone or Canbus net worked
  - These sketches can be used in various ways as shown in the video series.

### AVR2ServoNIO Sketch
  - Pin 2 is used for interrupt
  - Pin 10 CS SS (Slave Select) (used to select the slave device, also known as CS or Chip Select)
  - Pin 11 SI MOSI (Master Out Slave In)
  - Pin 12 SO MISO (Master In Slave Out)
  - Pin 13 SCK (Serial Clock)

  - Pins 4,5,6,7,A0,A1,A2,A3 are used for input or output
  - Pins A4 & A5 are the servo pins

You must have the arduino powered by a separate power supply other wise you will damage your PC com port.

### AVR4Servo8IO use the pins in a different order
  - Pins 4,5,6,7,8,9,A0,A1 are used for input or output
  - Pins A2,A3,A4,A5 are the servo pins
You must have the arduino powered by a separate power supply other wise you will damage your PC com port.

### AVR6Servo6IO use the pins in a different order
  - Pins 4,5,6,7,8,9 are used for input or output
  - Pins A0,A1,A2,A3,A4,A5 are the servo pins
You must have the arduino powered by a separate power supply other wise you will damage your PC com port.

### AVRNano13IO use the pins in a different order
  - Pins 3,4,5,6,7,8,9,A0,A1,A2,A3,A4,A5 are used for input or output
 

