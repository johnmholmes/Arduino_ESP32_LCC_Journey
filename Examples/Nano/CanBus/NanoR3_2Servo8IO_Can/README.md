# AVR 2-Servo 8-IO Example, 

This sketch was written for a Nano but now also includes the ARDUINO_UNOR4_MINIMA by change the board selection 
This sketch implements:
* two servos, each with three positions
   Positions cabe set to angles 0-180
   The 0 and 180 end-points of the servos can be configured.
* 8 i/o channels, each of which can be an input or an output,
   If an output it may be solid, pulse or flashing, and consumes an on- and 0ff-event. 
   If an input it produces an on- and off-event, which may each be delayed.  The 
     inputs can be with or with pullups, inverted or not, or can be toggle. 


Pin usage can be seen in the main file at the top. 


