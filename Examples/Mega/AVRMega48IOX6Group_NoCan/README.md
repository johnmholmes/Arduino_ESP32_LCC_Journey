# AVR 48-IO Example, 

This sketch was written for a Mega
This sketch implements:
* 48 i/o channels, each of which can be an input or an output,
   If an output it may be solid, pulse or flashing, and consumes an on- and 0ff-event. 
   If an input it produces an on- and off-event, which may each be delayed.  The 
     inputs can be with or with pullups, inverted or not, or can be toggle. 
 * Pin used are 3 - 49 + A0
 * Pins used for Can Bus are 2,50,51,52,53
 * We have 6 groups of 8 channels which should make it easier to configure.

Hardware 

Please note the board you see in the picture has a fault in its design and pin 21 does not work correctly.

![diymore](https://github.com/user-attachments/assets/652ead3b-30c4-4418-85ac-5b0c8188a793)


Here is the CDI view

![Screenshot 2025-03-30 103211](https://github.com/user-attachments/assets/7fac7f7c-d8e6-4684-a280-2d979d261bf8)


