//==============================================================
// AVR 24 Inputs and 24 output with actions using ACAN for Arduino Mega
//
// Copyright 2024 David P Harris
// Derived from work by Alex Shepherd and David Harris
// Updated 2024.11.14, modified 2025.09.01 for 63 inputs on Arduino Mega
//==============================================================
/* - 24 input channels (pins 14-37): not using i2c so pins 20 21 
     - type: 0=None,
             1=Input, 2=Input Inverted,
             3=Input with pull-up, 4=Input with pull-up Inverted,
             5=Input toggle, 6=Toggle with pull-up.
     - Events are produced
     - On-delay: delay before on-event is sent
     - Off-delay: the period before the off-event is sent
 - Reserved pins: 0, 1 (serial), 2 (interrupt), 50 (MISO), 51 (MOSI), 52 (SCK), 53 (CS for MCP2515)

MEGA_24 Output Node 
   24 channels, ie pins allows for 3 actions per pin.
   N Action eventids
   each:
   -- pin number from 38 to 49 + A0 to A14
   -- Actions from: Low, High, Flash, Double Strobe, Random, Fire
      Fire uses two consecutive pins/channels.
   -- two parameters: durn and rate
      -- each in steps of 100ms
      -- for fire, these determing the brightness of the two channels. 

EFFECTS:
 None
 Low: pulls the pin low (0V), the parameters make no change
 High: pushes the pin high (5V), the parameters make no change
 Flash: alternates between low and high
    Parameter 1: sets how long the pin is high (100-25.5 sec)
    Parameter 2: sets how long the pin is low (100-25.5 sec)
 Double strobe: puts the pin high, then low, then high, and then low.
    Parameter 1: how long the first low is held (100-25.5 sec)
    Parameter 2: how long the second low is held (100-25.5 sec)
 Random: alternates the pin between high and low
    Parameter 1: a random period the pin in held high, between 20ms and 20min
    Parameter 2: a random period the pin in held low, between 20ms and 20min
    Note the random period is between the half and the whol eof the chosen period.
    Eg: if the chosen period is 30 sec, then the random periods will be 
        between 25 sec and 30 sec.  
    This should work well for house lights for example.  
  Fire: this uses two consecutive pins. 
    It uses analogWtite to set the brightness of two LEDs attached to the pins.
    You may have to chose higher values to see an effect, as values below a 
    threshold may not light the LED.  
    Parameter 1: sets the randomness of the first pin of the pair 
    Parameter 2: sets the randomness of the second pin of the pair 


    PWM pins available 3, 4, 5, 6, 7, 8, 9, 44, 45, 46 not used in this sketch

    <relation><property>6</property><value>Fire (uses two pins)</value></relation> not used in this sketch.
*/


