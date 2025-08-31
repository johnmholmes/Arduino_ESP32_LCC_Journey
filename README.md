# Arduino Nano, Mega / ESP32 LCC Journey

# Please Use The Wiki for details

----

## OpenLCB_Single_Thread

- You need to have this library installed to be able to use these examples. It is available from within the Arduino Library Manager.

----

## 31 August/2025 Update

Added a new Mega sketch for NoCan testing which will be used for scenic lighting and mimic panel work on my layout later this year. We have 63 channels/pins 3 to 49 & A0-A15. Each can be taught to react to 3 events. The max number of events is 190 so you could use less for some pins and more for others if you wished.

----

## 3 August/2025 Update

The ESP32 examples are changing the first ones will be the Can bus versions. The is a change to the file structure which will be documented. We now have a config.h This has been done as the sketch is designed to run on various MCU's. I still plan to have set MCU version in this repository.

----


## Bug Reports 

Please raise bug reports via johnmarkholmes.jh@gmail.com

----


## Ensure if using the Grid Connect method check the Baud Rate setting in JMRI

After a two weeks of trying to understand why the AVR sketches did nit work, it turned out I had not ensured the Baud rate was set to 115200.

----


## The Arduino Nano and the MCP2515 are being pushed to there limits and may not be the best hardware to use!

- My goal is to present a selection of examples dedicated to the Arduino Nano and the MCP2515 module.
- In the future you will see me looking at maybe the ESP32.
- I will also colaberate with another MERG member to cover the Pi Pico versions.
- I will also introduce The Arduino Mega which gives us 53 usable GPIO pins.

----

## This repository is used to document my journey into using David's LCC library [examples](https://github.com/openlcb/OpenLCB_Single_Thread) Thanks to David Harris for making this possible for me.

- All credit goes to the library creators who have also helped me to get this going on an Arduino Nano using the MCP2515 modules, and use the MERG CANUSB4 to interface it with JMRI.
- Also we can use the example to create stand alone animation modules using just a USB lead.
- I will be using the Wiki feature rather than the readme file, so please click on the Wiki tab to open it.

----





