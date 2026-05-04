# Arduino Nano, Mega / ESP32 LCC Journey

It is important for people finding this repository to understand, these examples are not final products in anyway. I am not a professional programmer or a electronic hardware professional. While I try to make sure I follow best practise. I am very happy for people to offer any suggestions or corrections they feel are needed.

# Please Use The Wiki for more details

----

## OpenLCB_Single_Thread

- You need to have this library installed to be able to use these examples. It is available from within the Arduino Library Manager.

----

## Bug Reports 

Please raise bug reports via johnmarkholmes.jh@gmail.com

----

## April 18th 2026 Update

Test has started with the new sketch version for the 2 Servo 24 I/O. This has fixed an issue I was seeing with the servo speed setting.

Notes:-
- On the first configure of the node the servo will go to 90 degree very quickly, and lock in at 90 to allow for the servo and turnout to be installed.
- You can then move the slider to set your required closed position.
- To set the thrown position you will need to first trigger the thrown position, once this has been done you can then set the required position for the thrown.
- Once you have done this afterwards the servos will detach 4 seconds.

----

## March 28th 2026 Update

The respository is being updated as I finalise the sketches I will be using going forward with my layout. After lots of testing over the past year I will be only developing for the ESP32 Wroom 32. While this is coming to the end of its life in 2031. It will give us time to look at a suitable replacement while using the current sensor shield.

The ESP32 will allow for us to use 8 input/output and 2 native servoss as well as MCP23107 for extra I/O's. There has been testing with the PCA9685, however at this stage there are potential issues with some of the modules. Which needs to be fully understood. For My layout I do not use Frog switching for the turnouts.

So I will archive the Arduino Nano, Mega, R4 sketches in case people wish to take them forward in the future.

----







