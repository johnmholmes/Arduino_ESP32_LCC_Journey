# Arduino Nano / ESP32 LCC Journey

# Please Use The Wiki for details

## 06/2/2025 Update

New sketch variants come very soon for the AVR2ServoNIO currently on testing. & Wiki updated.

----

## Bug Reports

Please raise bug reports via johnmholmes@tiscali.co.uk

----

## ESP32 Nodes being added 7 Jan 2025

In 2025, with a deeper understanding of LCC and a strong interest in utilizing the CAN bus while creating standalone modules, I recognized the need to transition to the ESP32 to develop more capable and efficient modules. The ESP32's built-in CAN controller, requiring only a separate transceiver module, significantly reduces the GPIO usage for CAN bus communication. I plan to rework all my existing sketches to ensure compatibility with the ESP32 and take full advantage of its capabilities.

This does not mean that the Nano sketches will be removed from here, but may get modified to make them more useable for Can bus and standalone modules with limited functionalities.

----

## Ensure if using the Grid Connect method check the Baud Rate setting in JMRI

After a two weeks of trying to understand why the AVR sketches did nit work, it turned out I had not ensured the Baud rate was set to 115200.

----
## We now have many versions of the original AVR2ServoNIO sketch 

The main library as produced by David Harris I have now uploaded more versions for people to use with an Arduino Nano.

 - These cover Both CAN BUS use and USB use.
 - We have options for 2,4,6,8 servos.
 - We have 8,13,17 input or output option.

 - If you find any bugs please let me know so we can investigate them.

----

## The Arduino Nano and the MCP2515 are being pushed to there limits and may not be the best hardware to use!

- My goal is to present a selection of examples dedicated to the Arduino Nano and the MCP2515 module.
- In the future you will see me looking at maybe the ESP32.
- I will also colaberate with another MERG member to cover the Pi Pico versions.

----

## This repository is used to document my journey into using David's LCC library [examples](https://github.com/openlcb/OpenLCB_Single_Thread) Thanks to David Harris for making this possible for me.

- All credit goes to the library creators who have also helped me to get this going on an Arduino Nano using the MCP2515 modules, and use the MERG CANUSB4 to interface it with JMRI.
- Also we can use the example to create stand alone animation modules using just a USB lead.
- I will be using the Wiki feature rather than the readme file, so please click on the Wiki tab to open it.

----

## Version 0.1.7

This was released 10/Dec/2024





