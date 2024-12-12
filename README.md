# Arduino Nano LCC Journey

## Bug Highlighted 12 December 2024.

We have recieved notice of a bug which is effecting the servo speed change not working since the last bug fix. We are looking into it and will post a release once corrected and fully tested by myself.

The last bug fix was needed to fix a power up issue that could cause an issue causing turnout damage. This fix introduced the new bug which I had not spotted during testing.

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

- This repository will primarily focus on the Arduino Nano and Mega boards, as well as the MCP2515 CAN module.
- We currently have the buffer values set for 4 on the read and 2 on the transmit.


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

## Version 0.1.5

This was released 15/11/2024





