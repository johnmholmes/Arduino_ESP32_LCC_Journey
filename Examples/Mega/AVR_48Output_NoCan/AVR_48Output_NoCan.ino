/* AVR_48 Output Node 
   48 channels, ie pins
   N Action eventids
   each:
   -- pin number from 3 to 49 + A0
   -- Actions from: Low, High, Flash, Double Strobe, Random, Fire
      Fire uses two consecutive channels.
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
    Parameter 2: sets the randomness of the seocnd pin of the pair 

*/

//   David Harris 2019, adapted from
//   Bob Jacobsen 2010, 2012
//      based on examples by Alex Shepherd and David Harris
//   Updated 2024.09 DPH
//   Updated 2024.12. John Holmes
//==============================================================

// Debugging -- uncomment to activate debugging statements:
    // dP(x) prints x, dPH(x) prints x in hex,
    // dPS(string,x) prints string and x
//#define DEBUG Serial

// Allow direct to JMRI via USB, without CAN controller, comment out for CAN
//   Note: disable debugging if this is chosen
#include "GCSerial.h"

// New ACan for MCP2515
#define ACAN_FREQ 8000000UL   // set for crystal freq feeding the MCP2515 chip
#define ACAN_CS_PIN 10        // set for the MCP2515 chip select pin, usually 10 on Nano
#define ACAN_INT_PIN 2        // set for the MCP2515 interrupt pin, usually 2 or 3
// NOTE: see below for the #include "ACan/ACan.h" line and uncomment if ACan.h wanted.  

#define OLCB_NO_BLUE_GOLD  // don't want Blue/Gold

//************ USER DEFINITIONS ************************************

// Node ID --- this must come from a range controlled by the user.
// See: http://registry.openlcb.org/uniqueidranges
// To set a new NODEID edit the line below.
#define NODE_ADDRESS 5,1,1,1,0x8E,0xA1   // DIY range example, not for global use.

// Set to 1 to Force Reset to Factory Defaults, else 0.
// Must be done at least once for each new board.
#define RESET_TO_FACTORY_DEFAULTS 1

// Board definitions
#define MANU "OpenLCB"           // The manufacturer of node
#define MODEL "48 output"           // The model of the board
#define HWVERSION "0.1"          // Hardware version
#define SWVERSION "0.1"          // Software version

// Application definitions:
// For this example, set the number of channels implemented.
// Each corresponds to an input or output pin.
#define NUM_ACTION_SETS 14   // number of action sets
#define NUM_APS 10     // number of actions per sets
#define NUM_CHANNEL 48 // number of pins
#define NUM_ACTION 140  // number of events/actions
// Total number of eventids, in this case there are two per channel,
//  a set and unset.
#define NUM_EVENT NUM_ACTION

//************** End of USER DEFINTIONS *****************************
  
#include "mdebugging.h"           // debugging
#include "OpenLcbCore.h"
#include "OpenLCBHeader.h"        // System house-keeping.

//#include "ACan/ACan.h"            // uses local ACan class, comment out if using GCSerial
#define NOCAN                     // comment out if default CAN driver wanted

#include "processCAN.h"           // Auto-select CAN library
#include "processor.h"            // auto-selects the processor type, and CAN lib, EEPROM lib etc.

extern "C" {                      // the following are defined as external
  #define N(x) xN(x)     // allows the insertion of value (x)
  #define xN(x) #x       // .. into the CDI string.
// ===== CDI =====
//   Configuration Description Information in xml, **must match MemStruct below**
//   See: http://openlcb.com/wp-content/uploads/2016/02/S-9.7.4.1-ConfigurationDescriptionInformation-2016-02-06.pdf
//   CDIheader and CDIFooter contain system-parts, and includes user changable node name and description fields.
     const char configDefInfo[] PROGMEM =
       // vvvvv Enter User definitions below CDIheader line vvvvv
       //       It must match the Memstruct struct{} below
       CDIheader R"(
<group replication=')" N(NUM_ACTION_SETS) R"('>
  <name>Actions Groups</name>
  <repname>Action 1-10</repname>
  <repname>Action 11-20</repname>
  <repname>Action 21-30</repname>
  <repname>Action 31-40</repname>
  <repname>Action 41-50</repname>
  <repname>Action 51-60</repname>
  <repname>Action 61-70</repname>
  <repname>Action 71-80</repname>
  <repname>Action 81-90</repname>
  <repname>Action 91-100</repname>
  <repname>Action 101-110</repname>
  <repname>Action 111-120</repname>
  <repname>Action 121-130</repname>
  <repname>Action 131-140</repname>

  <group replication=')" N(NUM_APS) R"('>
    <name>Actions</name>
    <repname>Action</repname>
    <eventid><name>Event for action</name></eventid>
    <int size="1"><name>pin#</name>
        <map>
          <relation><property>0</property><value>None</value></relation>
          <relation><property>1</property><value>3</value></relation>
          <relation><property>2</property><value>4</value></relation>
          <relation><property>3</property><value>5</value></relation>
          <relation><property>4</property><value>6</value></relation>
          <relation><property>5</property><value>7</value></relation>
          <relation><property>6</property><value>8</value></relation>
          <relation><property>7</property><value>9</value></relation>
          <relation><property>8</property><value>10</value></relation>
          <relation><property>9</property><value>11</value></relation>
          <relation><property>10</property><value>12</value></relation>
          <relation><property>11</property><value>13</value></relation>
          <relation><property>12</property><value>14</value></relation>
          <relation><property>13</property><value>15</value></relation>
          <relation><property>14</property><value>16</value></relation>
          <relation><property>15</property><value>17</value></relation>
          <relation><property>16</property><value>18</value></relation>
          <relation><property>17</property><value>19</value></relation>
          <relation><property>18</property><value>20</value></relation>
          <relation><property>19</property><value>21</value></relation>
          <relation><property>20</property><value>22</value></relation>
          <relation><property>21</property><value>23</value></relation>
          <relation><property>22</property><value>24</value></relation>
          <relation><property>23</property><value>25</value></relation>
          <relation><property>24</property><value>26</value></relation>
          <relation><property>25</property><value>27</value></relation>
          <relation><property>26</property><value>28</value></relation>
          <relation><property>27</property><value>29</value></relation>
          <relation><property>28</property><value>30</value></relation>
          <relation><property>29</property><value>31</value></relation>
          <relation><property>30</property><value>32</value></relation>
          <relation><property>31</property><value>33</value></relation>
          <relation><property>32</property><value>34</value></relation>
          <relation><property>33</property><value>35</value></relation>
          <relation><property>34</property><value>36</value></relation>
          <relation><property>35</property><value>37</value></relation>
          <relation><property>36</property><value>38</value></relation>
          <relation><property>37</property><value>39</value></relation>
          <relation><property>38</property><value>40</value></relation>
          <relation><property>39</property><value>41</value></relation>
          <relation><property>40</property><value>42</value></relation>
          <relation><property>41</property><value>43</value></relation>
          <relation><property>42</property><value>44</value></relation>
          <relation><property>43</property><value>45</value></relation>
          <relation><property>44</property><value>46</value></relation>
          <relation><property>45</property><value>47</value></relation>
          <relation><property>46</property><value>48</value></relation>
          <relation><property>47</property><value>49</value></relation>
          <relation><property>48</property><value>A0</value></relation>
          </map>
    </int>
    <int size="1"><name>action</name>
        <map>
          <relation><property>0</property><value>None</value></relation>
          <relation><property>1</property><value>Low</value></relation>
          <relation><property>2</property><value>High</value></relation>
          <relation><property>3</property><value>Flash</value></relation>
          <relation><property>4</property><value>Double Strobe</value></relation>
          <relation><property>5</property><value>Random</value></relation>
          <relation><property>6</property><value>Fire (uses two pins)</value></relation>
        </map>
    </int>
    <int size="1"><name>Duration (100 ms)</name></int>
    <int size="1"><name>Rate (100 ms)</name></int>
  </group>

</group>          
       )" CDIfooter;
       // ^^^^^ Enter User definitions above CDIfooter line ^^^^^
}

// ===== MemStruct =====
//   Memory structure of EEPROM, **must match CDI above**
//     -- nodeVar has system-info, and includes the node name and description fields
    typedef struct {
          EVENT_SPACE_HEADER eventSpaceHeader; // MUST BE AT THE TOP OF STRUCT - DO NOT REMOVE!!!
          char nodeName[20];  // optional node-name, used by ACDI
          char nodeDesc[24];  // optional node-description, used by ACDI
      // vvvvv Enter User definitions below vvvvv
          
          struct {
            EventID eid;;       // Consumed eventID which sets this output-pin
            uint8_t pini;
            uint8_t action;
            uint8_t durn;
            uint8_t rate;
          } action[NUM_ACTION];
          
      // ^^^^^ Enter User definitions above ^^^^^
    } MemStruct;                 // EEPROM memory structure, must match the CDI above

extern "C" {
  
  // ===== eventid Table =====
  //  Array of the offsets to every eventID in MemStruct/EEPROM/mem, and P/C flags
  //    -- each eventid needs to be identified as a consumer, a producer or both.
  //    -- PEID = Producer-EID, CEID = Consumer, and PCEID = Producer/Consumer
  //    -- note matching references to MemStruct.
  // This line defines a useful macro to make filling the table easier

    #define GROUP(i) CEID(action[i].eid), \
                    CEID(action[i+1].eid), \
                    CEID(action[i+2].eid), \
                    CEID(action[i+3].eid), \
                    CEID(action[i+4].eid)

    const EIDTab eidtab[NUM_EVENT] PROGMEM = {
      GROUP(0), GROUP(5), GROUP(10), GROUP(15), 
      GROUP(20), GROUP(25), GROUP(30), GROUP(35), 
      GROUP(40), GROUP(45), GROUP(50), GROUP(55), 
      GROUP(60), GROUP(65), GROUP(70), GROUP(75),  
      GROUP(80), GROUP(85), GROUP(90), GROUP(95),  
      GROUP(100), GROUP(105), GROUP(110), GROUP(115),   
      GROUP(120), GROUP(125), GROUP(130), GROUP(135), 
      };
 
  // SNIP Short node description for use by the Simple Node Information Protocol
  // See: http://openlcb.com/wp-content/uploads/2016/02/S-9.7.4.3-SimpleNodeInformation-2016-02-06.pdf
    extern const char SNII_const_data[] PROGMEM = "\001" MANU "\000" MODEL "\000" HWVERSION "\000" OlcbCommonVersion ; // last zero in double-quote
    //extern const char SNII_const_data[] PROGMEM = "\001RailStars\000Io 8-Out 32-InOut 16-Servo\0001.0\0002.0" ; // last zero in double-quote
    ////extern const char SNII_const_data[] PROGMEM = "\001OpenLCB\0008Ouput\0001.0\0002.0\000"; // last zero in double-quote

}; // end extern "C"

// PIP Protocol Identification Protocol uses a bit-field to indicate which protocols this node supports
// See 3.3.6 and 3.3.7 in http://openlcb.com/wp-content/uploads/2016/02/S-9.7.3-MessageNetwork-2016-02-06.pdf
uint8_t protocolIdentValue[6] = {     // 0xD7,0x58,0x00,0,0,0};
        pSimple | pDatagram | pMemConfig | pPCEvents | !pIdent    | pTeach     | !pStream   | !pReservation, // 1st byte
        pACDI   | pSNIP     | pCDI       | !pRemote  | !pDisplay  | !pTraction | !pFunction | !pDCC        , // 2nd byte
        0, 0, 0, 0                                                                                           // remaining 4 bytes
};


#ifndef OLCB_NO_BLUE_GOLD
  // ===== Blue/Gold =====
  // Blue-gold refers to two standard buttons offering a rudimentary control ssystem for an node.
  //   Features: teaching/learning of eventids; node identification; node reset.

  // Board choices, each has differing i/o choices
    #include "boardChoices.h"

  // This section uses the ButtonLed lib to muliplex an input and output onto a single pin.
  // It includes sampling every 32 ms, and blink patterns.

  // Patterns
  // Each pattern is 32 bits, each bit is used sequencely to blink the LED on and off, at 64 ms per bit.
    #define ShortBlinkOn   0x00010001L
    #define ShortBlinkOff  0xFFFEFFFEL
    uint32_t patterns[NUM_EVENT] = { // two per channel, one per event
      ShortBlinkOn
    };
  //ButtonLed pA(14, LOW);
  // An array of buttons/leds.
    ButtonLed* buttons[NUM_EVENT] = {
       // One for each event; each channel is a pair
       &pA
    };
#endif // OLCB_NO_BLUE_GOLD   // this app uses ButtonLed lib for its i/o.



// ===== Process inputs and Producers eventids =====
// USER defined
uint8_t pin[NUM_CHANNEL] = { 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,A0};  // 13 channel
uint8_t currentEvent[NUM_CHANNEL];
uint8_t state[NUM_CHANNEL]; 
long timer[NUM_CHANNEL];

// ===== Process Consumer-eventIDs =====
// USER defined

#define PV(x) { dP(" " #x "="); dP(x); }
#define PVL(x) { dP("\n" #x "="); dP(x); }
void pceCallback(uint16_t index) {
  dP("\npceCallback("); dP(index);
  PV(NODECONFIG.read(EEADDR(action[index].pini))); 
  PV(NODECONFIG.read(EEADDR(action[index].action))); 
  PV(NODECONFIG.read(EEADDR(action[index].durn))); 
  PV(NODECONFIG.read(EEADDR(action[index].rate)));
  uint8_t i = NODECONFIG.read(EEADDR(action[index].pini)) - 1;  // correct to 0-index
  currentEvent[i] = index;
  timer[i] = millis() + NODECONFIG.read(EEADDR(action[index].durn));
  state[i] = 0;
  PV(i); PV(currentEvent[i]); PV(timer[i]); PV(state[i]);
}

/* Config tool:
 *  More: Reset/Reboot -- causes a reboot --> reads nid, and reads and sorts eids --> userInit();
 *  More: Update Complete -- no reboot --> reads and sorts eids --> userInit();
 *  Reset segment: Usr clear -- new set of new set of eids and blank strings (in system code), doesn't have to reboot --> userClear();
 *  Reset segment: Mfr clear -- original set of eids, blank strings --> userMfrClear().
 *
 */

// userInitAll() -- include any initialization after Factory reset "Mfg clear" or "User clear"
//  -- clear or pre-define text variables.
// USER defined
bool initialized = false;
void userInitAll() {
  NODECONFIG.put(EEADDR(nodeName), ESTRING("AVR Mega"));
  NODECONFIG.put(EEADDR(nodeDesc), ESTRING("48 Outputs"));
  for(int i=0; i<NUM_ACTION; i++) NODECONFIG.write(EEADDR(action[i].pini), 0);
  for(int i=0; i<NUM_CHANNEL; i++) {
    pinMode(pin[i], OUTPUT);
    state[i]=0;
    timer[i]=0;
    currentEvent[i]=255;
  }
  initialized = true;
}

// userSoftReset() - include any initialization after a soft reset, ie after configuration changes.
// USER defined
void userSoftReset() {
  //dP("\n In userSoftReset()"); Serial.flush();
  REBOOT;  // defined in processor.h for each mpu
}

// userHardReset() - include any initialization after a hard reset, ie on boot-up.
// USER defined
void userHardReset() {
  //dP("\n In userHardReset()"); Serial.flush();
  REBOOT;  // defined in processor.h for each mpu
}

// ===== Callback from a Configuration write =====
// Use this to detect changes in the node's configuration
// This may be useful to take immediate action on a change.
// param address - address in space of change
// param length  - length of change
// NB: if address=0 and length==0xffff, then user indicated UPDATE_COMPLETE
//
// USER defined
void userConfigWritten(uint32_t address, uint16_t length, uint16_t func) {
}

#include "OpenLCBMid.h"           // System house-keeping

enum Action { aLOW=1, aHIGH, aFLASH, aDSTROBE, aRANDOM, aFIRE, aFIREOFF };
void doActions() {
  static uint8_t i = 255;
  uint8_t pi, actn, rate, durn; 
  //static long last = 0;
  //if( (now-last) <(50/NUM_CHANNEL) ) return;
  //last = now;
  if(!initialized) return;
  if(++i>=NUM_CHANNEL) i=0;
  uint8_t ei = currentEvent[i];
  if(ei==255) return;
  long now = millis();
  //PVL(i); PV(ei);
  pi = NODECONFIG.read(EEADDR(action[ei].pini))-1;  // pin number
  actn = NODECONFIG.read(EEADDR(action[ei].action));
  durn = NODECONFIG.read(EEADDR(action[ei].durn));
  rate = NODECONFIG.read(EEADDR(action[ei].rate));
  //PVL(pi); PV(pin[pi]); PV(actn); PV(durn); PV(rate); PV(state[i]); 
  switch (actn) {
    case aLOW: 
      if(state[i]) return;
      dP("\naLOW"); 
      digitalWrite(pin[pi], LOW); 
      state[i] = 1;
      break;
    case aHIGH: 
      if(state[i]) return;
      dP("\naHIGH"); 
      digitalWrite(pin[pi], HIGH); 
      state[i] = 1;
      break;
    case aFLASH:
      if( timer[i] && (now-timer[i])>0 ) {
        dP("\naFLASH"); 
        switch ( state[i] ) {
          case 0:
            digitalWrite(pin[pi], HIGH);
            timer[i] = now+durn*100;
            state[i] = 1;
            break;
          case 1:
            digitalWrite(pin[pi], LOW);
            timer[i] = now+rate*100;
            state[i] = 0;
            break;
        }
      }
      break;
    case aDSTROBE:
      if( timer[i] && (now-timer[i])>0 ) {
        dP("\naDSTROBE"); PV(state[i]);
        switch( state[i] ) {
          case 0:
            digitalWrite(pin[pi], HIGH);
            timer[i] = now+100;
            PV(timer[i]);
            state[i] = 1;
            break;
          case 1:
            digitalWrite(pin[pi], LOW);
            timer[i] = now+durn*100;
            PV(timer[i]);
            state[i] = 2;
            break;
          case 2:
            digitalWrite(pin[pi], HIGH);
            timer[i] = now+100;
            PV(timer[i]);
            state[i] = 3;
            break;
          case 3:
            digitalWrite(pin[pi], LOW);
            timer[i] = now+rate*100;
            PV(timer[i]);
            state[i] = 0;
            break;
        }
      }
      break;
    case aRANDOM:
      if( timer[i] && (now-timer[i])>0 ) {
        dP("\naRANDOM"); PV(state[i]);
        switch( state[i] ) {
          case 0:
            digitalWrite(pin[pi], HIGH);
            timer[i] = now+random(durn*durn*5, durn*durn*20);
            PV(timer[i]);
            state[i] = 1;
            break;
          case 1:
            digitalWrite(pin[pi], LOW);
            timer[i] = now+random(rate*rate*5, rate*rate*20);
            PV(timer[i]);
            state[i] = 0;
            break;
        }
      }
      break;
    case aFIRE:
      if(state[i] != 0) return; 
      if(durn==0) {
        analogWrite(pin[pi], 0);
        analogWrite(pin[pi+1], 0);
        state[i] = 1;
      }
      if( timer[i] && (now-timer[i])>0 ) {
        dP("\naFIRE"); dP(pin[pi]);
        if(pi+1>NUM_CHANNEL) break;
        analogWrite(pin[pi], random(durn));
        analogWrite(pin[pi+1], random(rate));
        timer[i] = now+15;
      }
      break;
  }
}


// ==== Setup does initial configuration =============================
void setup() {
  #ifdef DEBUG
    Serial.begin(115200); while(!Serial); delay(500); Serial.print("\n Testing 48 output"); 
    dP("\nSize of MemStruct= "); dP(sizeof(MemStruct)); 
  #endif

  NodeID nodeid(NODE_ADDRESS);       // this node's nodeid
  Olcb_init(nodeid, RESET_TO_FACTORY_DEFAULTS);
  // set output pins to channel
  for(int c=0; c<NUM_CHANNEL; c++) {
    pinMode( pin[c], OUTPUT);
  }

  #if 1
    // test values
    NODECONFIG.write(EEADDR(action[0].pini), 1);
    NODECONFIG.write(EEADDR(action[0].action), aLOW);

    NODECONFIG.write(EEADDR(action[1].pini), 1);
    NODECONFIG.write(EEADDR(action[1].action), aHIGH);

    NODECONFIG.write(EEADDR(action[2].pini), 1);
    NODECONFIG.write(EEADDR(action[2].action), aFLASH);
    NODECONFIG.write(EEADDR(action[2].durn), 10);
    NODECONFIG.write(EEADDR(action[2].rate), 10);

    NODECONFIG.write(EEADDR(action[3].pini), 1);
    NODECONFIG.write(EEADDR(action[3].action), aDSTROBE);
    NODECONFIG.write(EEADDR(action[3].durn), 2);
    NODECONFIG.write(EEADDR(action[3].rate), 10);
    
    NODECONFIG.write(EEADDR(action[4].pini), 1);
    NODECONFIG.write(EEADDR(action[4].action), aRANDOM);
    NODECONFIG.write(EEADDR(action[4].durn), 20);
    NODECONFIG.write(EEADDR(action[4].rate), 20);
    
    NODECONFIG.write(EEADDR(action[5].pini), 1);
    NODECONFIG.write(EEADDR(action[5].action), aRANDOM);
    NODECONFIG.write(EEADDR(action[5].durn), 5);
    NODECONFIG.write(EEADDR(action[5].rate), 3);
    
    NODECONFIG.write(EEADDR(action[6].pini), 1);
    NODECONFIG.write(EEADDR(action[6].action), aFIRE);
    NODECONFIG.write(EEADDR(action[6].durn), 135);
    NODECONFIG.write(EEADDR(action[6].rate), 160);

    NODECONFIG.write(EEADDR(action[7].pini), 1);
    NODECONFIG.write(EEADDR(action[7].action), aFIRE);
    NODECONFIG.write(EEADDR(action[7].durn), 0);
    NODECONFIG.write(EEADDR(action[7].rate), 0);

    #if 0
    for(uint8_t index=0; index<10; index++) {
      uint8_t pini = NODECONFIG.read(EEADDR(action[index].pini));
      uint8_t action = NODECONFIG.read(EEADDR(action[index].action));
      uint8_t durn = NODECONFIG.read(EEADDR(action[index].durn));
      uint8_t rate = NODECONFIG.read(EEADDR(action[index].rate));
      PVL(index); PV(pini); PV(action); PV(durn); PV(rate);
    }
    for(int i=0; i<NUM_CHANNEL; i++) {
      //PVL(i); PV(currentEvent[i]); PV(timer[i]); PV(state[i]);
    }
    #endif
  #endif
}


// ==== MAIN LOOP ===========================================
//  -- this performs system functions, such as CAN alias maintenence
void loop() {
  
  bool activity = Olcb_process();     // System processing happens here, with callbacks for app action.

  doActions();
} 