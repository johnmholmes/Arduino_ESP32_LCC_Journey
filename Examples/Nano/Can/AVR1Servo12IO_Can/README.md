# AVR 1-Servo 12-IO Example, 

This sketch was written for a Nano 
This sketch implements:
* One servos, each with three positions
   Positions cabe set to angles 0-180
   The 0 and 180 end-points of the servos can be configured.
* 12 i/o channels, each of which can be an input or an output,
   If an output it may be solid, pulse or flashing, and consumes an on- and 0ff-event. 
   If an input it produces an on- and off-event, which may each be delayed.  The 
     inputs can be with or with pullups, inverted or not, or can be toggle.
* It has EEprom saving of the current position which can be set within the sketch. It is set to record this every 30 minutes and only if the state has changed. I will be making this value longer for my layout so maybe 4 hours, as the more often you write to the EEprom the faster its life will be lost. Every 30 minutes will give you about 5 years if the layout is used every day.


I/O Pin usage 3,4,5,6,7,8,9,A0,A1,A2,A3,A4 

Servo pin usage A5

Can Bus Pins 2,10,11,12,13

Hardware

![Screenshot 2025-02-19 091722](https://github.com/user-attachments/assets/efde63fd-0246-4bcf-a99f-d70ff8018740)

