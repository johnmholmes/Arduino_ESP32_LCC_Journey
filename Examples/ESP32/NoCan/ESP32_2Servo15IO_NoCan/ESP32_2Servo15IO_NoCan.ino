//2025.08.25 changes: Can Transceiver Version Only
//  Use update and update16 instead of write to reduce EEPROM wear
//  Moved initialization of curpos (may have been overwriting things!)
/*
This is my test version for demonstration CAN Bus use only by John Holmes
  - Pins 32 & 26 RX and TX for the transceiver module
  - Pins 14,27,25,33,21,22,15,2,4,16,17,5,18,19,23  are used for input or output
  - Pins 12,13 servos

*/
//==============================================================
// AVR 2Servos NIO using ESPcan
//
// Coprright 2024 David P Harris
// derived from work by Alex Shepherd and David Harris
// Updated 2024.11.14
//==============================================================
// - 2 Servo channels, each with 
//     - three settable positions
//     - three set position events 
// - N input/output channels:
//     - type: 0=None, 
//             1=Input, 2=Input inverted, 
//             3=Input with pull-up, 4=Input with pull-up inverted, 
//             5=Input toggle, 6=Toggle with pull-up
//             7=Output, 8=Output inverted.
//     - for Outputs: 
//       - Events are consumed
//       - On-duration: how long the output is set, from 10ms - 2550ms, 0 means forever
//       - Off-period: the period until a repeat pulse, 0 means no repeat
//     - for Inputs:
//       - Events are produced
//       - On-delay: delay before on-event is sent
//       - Off-delay: the period before the off-event is sent
//
//==============================================================

// Debugging -- uncomment to activate debugging statements:
//#define DEBUG Serial

// Allow direct to JMRI via USB, without CAN controller, comment out for CAN
//    ( Note: disable debugging if this is chosen. )
#include "GCSerial.h"
#define NOCAN

#define ESPcan_RXPIN 32
#define ESPcan_TXPIN 26

// Board definitions
#define MANU "OpenLCB"           // The manufacturer of node
#define MODEL "ESP32_2Servo15IO"  // The model of the board
#define HWVERSION "0.1"          // Hardware version
#define SWVERSION "0.1"          // Software version

// To set a new nodeid edit the next line
#define NODE_ADDRESS  5,1,1,1,0x8E,0x02

// To Force Reset EEPROM to Factory Defaults set this value to 1, else 0.
// Need to do this at least once.
#define RESET_TO_FACTORY_DEFAULTS 1

// If you want the servopositions save, every x ms, set SAVEPERIOD > 0. 
// 300000 would be every 30 minutes, the EEPROM should last > 5 years. 
// Note: a write will not be done when the servo position has not changed. 
#define SAVEPERIOD 300000

// User defs
#define NUM_SERVOS 2
#define NUM_POS 3
#define NUM_IO 15

#define NUM_EVENT NUM_SERVOS * NUM_POS + NUM_IO*2

#include "mdebugging.h"           // debugging
#include "processCAN.h"           // Auto-select CAN library
#include "processor.h"            // auto-selects the processor type, EEPROM lib etc.
#include "OpenLCBHeader.h"        // System house-keeping.

// CDI (Configuration Description Information) in xml, must match MemStruct
// See: http://openlcb.com/wp-content/uploads/2016/02/S-9.7.4.1-ConfigurationDescriptionInformation-2016-02-06.pdf
extern "C" {
    #define N(x) xN(x)     // allow the insertion of the value (x) ..
    #define xN(x) #x       // .. into the CDI string.
const char configDefInfo[] PROGMEM =
// ===== Enter User definitions below =====
  CDIheader R"(
    <name>Application Configuration</name>
    <group>
        <name>Turnout Servo Configuration</name>
        <int size='1'>
          <name>Speed 5-50 (delay between steps)</name>
          <min>5</min><max>50</max>
          <hints><slider tickSpacing='15' immediate='yes' showValue='yes'> </slider></hints>
        </int>
    </group>
    <group replication=')" N(NUM_SERVOS) R"('>
        <name>Servos</name>
        <repname>Servo </repname>
        <string size='8'><name>Description</name></string>
        <group replication=')" N(NUM_POS) R"('>
        <name>  Closed     Midpoint   Thrown</name>
            <repname>Position </repname>
            <eventid><name>EventID</name></eventid>
            <int size='1'>
                <name>Servo Position in Degrees</name>
                <min>0</min><max>180</max>
                <hints><slider tickSpacing='45' immediate='yes' showValue='yes'> </slider></hints>
            </int>
        </group>
    </group>
    <group replication=')" N(NUM_IO) R"('>
        <name>Input/Output</name>
        <repname>IO </repname>
        <string size='8'><name>Description</name></string>
        <int size='1'>
            <name>Channel type</name>
            <map>
                <relation><property>0</property><value>None</value></relation> 
                <relation><property>1</property><value>Input</value></relation> 
                <relation><property>2</property><value>Input Inverted</value></relation> 
                <relation><property>3</property><value>Input with pull-up</value></relation>
                <relation><property>4</property><value>Input with pull-up Inverted</value></relation>
                <relation><property>5</property><value>Toggle</value></relation>
                <relation><property>6</property><value>Toggle with pull-up</value></relation>
                <relation><property>7</property><value>Output PA</value></relation>
                <relation><property>8</property><value>Output PA Inverted</value></relation>
                <relation><property>9</property><value>Output PB</value></relation>
                <relation><property>10</property><value>Output PB Inverted</value></relation>
            </map>
        </int>
        <int size='1'>
            <name>On-Duration/On-delay 1-255 = 100ms-25.5s, 0=steady-state</name>
            <hints><slider tickSpacing='50' immediate='yes' showValue='yes'> </slider></hints>
        </int>
        <int size='1'>
            <name>Off-Period/Off-delay 1-255 = 100ms-25.5s, 0=No repeat</name>
            <hints><slider tickSpacing='50' immediate='yes' showValue='yes'> </slider></hints>
        </int>
        <eventid><name>On-Event</name></eventid>
        <eventid><name>Off-Event</name></eventid>
    </group>
    )" CDIfooter;
// ===== Enter User definitions above =====
} // end extern

// ===== MemStruct =====
//   Memory structure of EEPROM, must match CDI above
    typedef struct {
          EVENT_SPACE_HEADER eventSpaceHeader; // MUST BE AT THE TOP OF STRUCT - DO NOT REMOVE!!!
          char nodeName[20];  // optional node-name, used by ACDI
          char nodeDesc[24];  // optional node-description, used by ACDI
      // ===== Enter User definitions below =====
          uint8_t servodelay;
          struct {
            char desc[8];        // description of this Servo Turnout Driver
            struct {
              EventID eid;       // consumer eventID
              uint8_t angle;     // position
            } pos[NUM_POS];
          } servos[NUM_SERVOS];
          struct {
            char desc[8];
            uint8_t type;
            uint8_t duration;    // 100ms-25.5s, 0=solid
            uint8_t period;      // 100ms-25.5s, 0=no repeat
            EventID onEid;
            EventID offEid;
          } io[NUM_IO];
      // ===== Enter User definitions above =====
      uint8_t curpos[NUM_SERVOS];  // save current positions of servos
      // items below will be included in the EEPROM, but are not part of the CDI
    } MemStruct;                 // type definition

uint8_t curpos[NUM_SERVOS]; 


extern "C" {
    // ===== eventid Table =====
    #define REG_SERVO_OUTPUT(s) CEID(servos[s].pos[0].eid), CEID(servos[s].pos[1].eid), CEID(servos[s].pos[2].eid)
    #define REG_IO(i) PCEID(io[i].onEid), PCEID(io[i].offEid)
    
    //  Array of the offsets to every eventID in MemStruct/EEPROM/mem, and P/C flags
    const EIDTab eidtab[NUM_EVENT] PROGMEM = {
        REG_SERVO_OUTPUT(0), REG_SERVO_OUTPUT(1),
        REG_IO(0), REG_IO(1), REG_IO(2), REG_IO(3), REG_IO(4), REG_IO(5), REG_IO(6), REG_IO(7), 
        REG_IO(8), REG_IO(9), REG_IO(10), REG_IO(11), REG_IO(12), REG_IO(13), REG_IO(14), 
    };
    
    // SNIP Short node description for use by the Simple Node Information Protocol
    // See: http://openlcb.com/wp-content/uploads/2016/02/S-9.7.4.3-SimpleNodeInformation-2016-02-06.pdf
    extern const char SNII_const_data[] PROGMEM = "\001" MANU "\000" MODEL "\000" HWVERSION "\000" OlcbCommonVersion ; // last zero in double-quote
} // end extern "C"

// PIP Protocol Identification Protocol uses a bit-field to indicate which protocols this node supports
// See 3.3.6 and 3.3.7 in http://openlcb.com/wp-content/uploads/2016/02/S-9.7.3-MessageNetwork-2016-02-06.pdf
uint8_t protocolIdentValue[6] = {   //0xD7,0x58,0x00,0,0,0};
        pSimple | pDatagram | pMemConfig | pPCEvents | !pIdent    | pTeach     | !pStream   | !pReservation, // 1st byte
        pACDI   | pSNIP     | pCDI       | !pRemote  | !pDisplay  | !pTraction | !pFunction | !pDCC        , // 2nd byte
        0, 0, 0, 0                                                                                           // remaining 4 bytes
    };

#define OLCB_NO_BLUE_GOLD  // blue/gold not used in this sketch
#ifndef OLCB_NO_BLUE_GOLD
    #define BLUE 40  // built-in blue LED
    #define GOLD 39  // built-in green LED
    ButtonLed blue(BLUE, LOW);
    ButtonLed gold(GOLD, LOW);
    
    uint32_t patterns[8] = { 0x00010001L, 0xFFFEFFFEL }; // two per channel, one per event
    ButtonLed pA(13, LOW);
    ButtonLed pB(14, LOW);
    ButtonLed pC(15, LOW);
    ButtonLed pD(16, LOW);
    ButtonLed* buttons[8] = { &pA,&pA,&pB,&pB,&pC,&pC,&pD,&pD };
#endif // OLCB_NO_BLUE_GOLD

//#include <Servo.h>
#include <ESP32Servo.h>
Servo servo[2];
uint8_t servodelay;
uint8_t servopin[NUM_SERVOS] = {12,13};
uint8_t servoActual[NUM_SERVOS];
uint8_t servoTarget[NUM_SERVOS];
#ifdef NOCAN
  uint8_t iopin[NUM_IO] = {14,27,25,33,21,22,15,2,4,16,17,5,18,19,23 }; 
#else
  uint8_t iopin[NUM_IO] = {14,27,25,33,21,22,15,2,4,16,17,5,18,19,23 };  
#endif
bool iostate[NUM_IO] = {0};  // state of the iopin
bool logstate[NUM_IO] = {0}; // logic state for toggle
unsigned long next[NUM_IO] = {0};

// This is called to initialize the EEPROM to Factory Reset
void userInitAll()
{ 
  NODECONFIG.put(EEADDR(nodeName), ESTRING("Esp32 Can"));
  NODECONFIG.put(EEADDR(nodeDesc), ESTRING("2Servos15IO"));
  NODECONFIG.update(EEADDR(servodelay), 50);
  for(uint8_t i = 0; i < NUM_SERVOS; i++) {
    NODECONFIG.put(EEADDR(servos[i].desc), ESTRING(""));
    for(int p=0; p<NUM_POS; p++) {
      NODECONFIG.update(EEADDR(servos[i].pos[p].angle), 90);
      NODECONFIG.update(EEADDR(curpos[i]), 0);
    }
  }
  for(uint8_t i = 0; i < NUM_IO; i++) {
    NODECONFIG.put(EEADDR(io[i].desc), ESTRING(""));
    NODECONFIG.update(EEADDR(io[i].type), 0);
    NODECONFIG.update(EEADDR(io[i].duration), 0);
    NODECONFIG.update(EEADDR(io[i].period), 0);
  }  
}

// determine the state of each eventid
enum evStates { VALID=4, INVALID=5, UNKNOWN=7 };
uint8_t userState(uint16_t index) {
  //dP("\n userState "); dP((uint16_t) index);
    if(index < NUM_SERVOS*NUM_POS) {
      int ch = index/3;
      int pos = index%3;
      //dP( (uint8_t) curpos[ch]==pos);
      if( curpos[ch]==pos ) return VALID;
                else return INVALID;
    } else {
      int ch = (index-NUM_SERVOS*NUM_POS)/2;
      if( NODECONFIG.read(EEADDR(io[ch].type))==0) return UNKNOWN;
      int evst = index % 2;
      //dP((uint8_t) iostate[ch]==evst);
      if( iostate[ch]==evst ) return VALID;
    }
    return INVALID;
}
    
// ===== Process Consumer-eventIDs =====
void pceCallback(uint16_t index) {
// Invoked when an event is consumed; drive pins as needed
// from index of all events.
// Sample code uses inverse of low bit of pattern to drive pin all on or all off.
// The pattern is mostly one way, blinking the other, hence inverse.
//
  #define PV(x) { dP(" " #x "="); dP(x); }
  dP("\npceCallback, index="); dP((uint16_t)index);
    if(index<NUM_SERVOS*NUM_POS) {
      uint8_t outputIndex = index / 3;
      uint8_t outputState = index % 3;
      curpos[outputIndex] = outputState;
      servo[outputIndex].attach(servopin[outputIndex]);
      servoTarget[outputIndex] = NODECONFIG.read( EEADDR(servos[outputIndex].pos[outputState].angle) );
    } else {
      uint8_t n = index-NUM_SERVOS*NUM_POS;
      uint8_t type = NODECONFIG.read(EEADDR(io[n/2].type));
      dP("\nevent"); PV(n); PV(type);
      if(type>=7) {
        // 7=PA 8=PAI 9=PB 10=PBI
        bool inv = !(type&1);       // inverted
        if(n%2) {
          //dP("\noff"); PV(n); PV(iopin[n/2]); PV(type); PV(inv);
          digitalWrite( iopin[n/2], inv);
          next[n/2] = 0;
        } else {
          bool pha = type<9;       // phaseA
          //dP("\ndw!"); PV(iopin[n/2]); PV(pha); PV(inv); 
          digitalWrite( iopin[n/2], pha ^ inv);
          iostate[n/2] = 1;
          uint8_t durn = NODECONFIG.read(EEADDR(io[n/2].duration));
          if(durn) next[n/2] = millis() + 100*durn; // note duration==0 means forever
          else next[n/2]=0;
            PV(millis()); PV(next[n/2]);
        }
      }
    }
}

// === Process servos ===
// This is called from loop to service the servos
void servoProcess() {
  static long last = 0;
  if( (millis()-last) < servodelay ) return;
  last = millis();
  for(int i=0; i<NUM_SERVOS; i++) {
    if(servoTarget[i] > servoActual[i] ) {
      if(!servo[i].attached()) servo[i].attach(servopin[i]);
      servo[i].write(servoActual[i]++);
    } else if(servoTarget[i] < servoActual[i] ) {
      if(!servo[i].attached()) servo[i].attach(servopin[i]); 
      servo[i].write(servoActual[i]--);
    } else if(servo[i].attached()) servo[i].detach(); 
  }
  #if SAVEPERIOD > 0
    static long lastsave = 0;
    if( (millis()-lastsave) > SAVEPERIOD ) {
      lastsave = millis();
      for(int i=0; i<NUM_SERVOS; i++) 
        NODECONFIG.update( EEADDR(curpos[i]), curpos[i]);
      dP("\n Pos saved");
    }
  #endif

}

// ==== Process Inputs ====
void produceFromInputs() {
    // called from loop(), this looks at changes in input pins and
    // and decides which events to fire
    // with pce.produce(i);
    const uint8_t base = NUM_SERVOS*NUM_POS;
    static uint8_t c = 0;
    static unsigned long last = 0;
    if((millis()-last)<(50/NUM_IO)) return;
    last = millis();
    uint8_t type = NODECONFIG.read(EEADDR(io[c].type));
    uint8_t d;
    if(type==5 || type==6) {
      bool s = digitalRead(iopin[c]);
      if(s != iostate[c]) {
        iostate[c] = s;
        if(!s) {
          logstate[c] ^= 1;
          if(logstate[c]) d = NODECONFIG.read(EEADDR(io[c].duration));
          else            d = NODECONFIG.read(EEADDR(io[c].period));
          //dP("\ninput "); PV(c); PV(type); PV(s); PV(logstate[c]); PV(d);
          if(d==0) OpenLcb.produce( base+c*2 + logstate[c] ); // if no delay send the event
          else next[c] = millis() + (uint16_t)d*100;          // else register the delay
          //PV(millis()); PV(next[c]);
        }
      }
    }
    if(type>0 && type<5) {
      bool s = digitalRead(iopin[c]);
      if(s != iostate[c]) {
        iostate[c] = s;
        if(!iostate[c]) d = NODECONFIG.read(EEADDR(io[c].duration)); 
        else d = NODECONFIG.read(EEADDR(io[c].period));
        //dP("\ninput "); PV(type); PV(s); PV(d);
        if(d==0) OpenLcb.produce( base+c*2 + (!s^(type&1)) ); // if no delay send event immediately
        else {
          next[c] = millis() + (uint16_t)d*100;                   // else register the delay
          //PV(millis()); PV(next[c]);
        }
      }
    }
    if(++c>=NUM_IO) c = 0;
}

// Process pending producer events
// Called from loop to service any pending event waiting on a delay
void processProducer() {
  const uint8_t base = NUM_SERVOS*NUM_POS;
  static unsigned long last = 0;
  unsigned long now = millis();
  if( (now-last) < 50 ) return;
  for(int c=0; c<NUM_IO; c++) {
    if(next[c]==0) continue;
    if(now<next[c]) continue; PV(c);
    uint8_t type = NODECONFIG.read(EEADDR(io[c].type));
    if(type>6) return; // do not process outputs
    uint8_t s = iostate[c];
    //dP("\nproducer"); PV(type); PV(s); PV((!s^(type&1)));
    if(type<5)  OpenLcb.produce( base+c*2 + (!s^(type&1)) ); // reg inputs
    else OpenLcb.produce( base+c*2 + logstate[c] );          // toggle inputs
    next[c] = 0;
  }
}

void userSoftReset() {}
void userHardReset() {}

#include "OpenLCBMid.h"    // Essential, do not move or delete

#if 0
#define P(...) Serial.print( __VA_ARGS__)
#define PV(x) { P(" "); P(#x  "="); P(x); }
#define PVL(x) { P("\n"); P(#x  "="); P(x); }
void printMem() {
  PVL(NODECONFIG.read(EEADDR(servodelay)));
  for(int s=0;s<NUM_SERVOS;s++) {
    P("\nServo "); P(s);
    for(int p=0;p<NUM_POS;p++) {
      PV(NODECONFIG.read(EEADDR(servos[s].pos[p])));
    }
  }
  /*
  for(int i=0;s<NUM_IO;i++) {
    P("\nIO "); P(i);
    PV(NODECONFIG.read(EEADDR(io[i].type)));
    PV(NODECONFIG.read(EEADDR(io[i].duration)));
    PV(NODECONFIG.read(EEADDR(io[i].period)));
  }
  */
}
#endif


// Callback from a Configuration write
// Use this to detect changes in the ndde's configuration
// This may be useful to take immediate action on a change.
void userConfigWritten(uint32_t address, uint16_t length, uint16_t func)
{
  dPS("\nuserConfigWritten: Addr: ", (uint32_t)address);
  dPS(" Len: ", (uint16_t)length);
  dPS(" Func: ", (uint8_t)func);
  setupIOPins();
  servoSet();
}

// Reinitialize servos to their current positions
// Called from setup() and after every configuration change
void servoSetup() {
  for(uint8_t i = 0; i < NUM_SERVOS; i++) {
    #if SAVEPERIOD > 0
      curpos[i] = NODECONFIG.read( EEADDR( curpos[i] ) ); 
    #else 
      curpos[i] = 0;
      dP("\nset curpos=0");
    #endif
    //servoActual[i] = 90;
    servoActual[i] = NODECONFIG.read(EEADDR(servos[i].pos[1].angle)); // orginal 0 trying 1
    servo[i].attach(servopin[i]);
    servo[i].write( servoActual[i] );
    dP("\n actual="); dP(servoActual[i]);
  }
  servoSet();
}
// Allow Servo adjustments
void servoSet() {
  servodelay = NODECONFIG.read( EEADDR( servodelay) );
  for(uint8_t i = 0; i < NUM_SERVOS; i++) {
    //uint8_t cpos = NODECONFIG.read( EEADDR(curpos[i]) );
    uint8_t cpos = curpos[i];
    dP("\n cpos="); dP(cpos);
    servoTarget[i] = NODECONFIG.read( EEADDR(servos[i].pos[cpos].angle) );
    dP("\n target="); dP(servoTarget[i]);
  }
}

// Setup the io pins
// called by setup() and after a configuration change
void setupIOPins() {
  dP("\nPins: ");
  for(uint8_t i=0; i<NUM_IO; i++) {
    uint8_t type = NODECONFIG.read( EEADDR(io[i].type));
    switch (type) {
      case 1: case 2: case 5:
        dP(" IN:");
        pinMode(iopin[i], INPUT); 
        iostate[i] = type&1;
        if(type==5) iostate[i] = 0;
        break;
      case 3: case 4: case 6:
        dP(" INP:");
        pinMode(iopin[i], INPUT_PULLUP); 
        iostate[i] = type&1;
        break;
      case 7: case 8: case 9: case 10:
        dP(" OUT:");
        pinMode(iopin[i], OUTPUT); 
        iostate[i] = !type&1;
        digitalWrite(iopin[i], !type&1);
        break;
    }
    dP(iopin[i]); dP(":"); dP(type); dP(", ");
  }
}

// Process IO pins
// called by loop to implement flashing on io pins
void appProcess() {
  uint8_t base = NUM_SERVOS * NUM_POS;
  unsigned long now = millis();
  for(int i=0; i<NUM_IO; i++) {
    uint8_t type = NODECONFIG.read(EEADDR(io[i].type));
    if(type >= 7) {
      if( next[i] && now>next[i] ) {
        //dP("\nappProcess "); PV(now);
        bool inv = !(type&1);
        bool phb = type>8;
        if(iostate[i]) {
          // phaseB
          dP("\nphaseB"); PV(phb); PV(inv); PV(iopin[i]); PV(phb ^ inv);
          digitalWrite(iopin[i], phb ^ inv);
          iostate[i] = 0;
          if( NODECONFIG.read(EEADDR(io[i].period)) > 0 ) 
          next[i] = now + 100*NODECONFIG.read(EEADDR(io[i].period));
          else next[i] = 0;
            PV(next[i]);
        } else {
          // phaseA
          dP("\nphaseA"); PV(phb); PV(inv); PV(!phb ^ inv);
          digitalWrite(iopin[i], !phb ^ inv);
          iostate[i] = 1;
          if( NODECONFIG.read(EEADDR(io[i].duration)) > 0 )
            next[i] = now + 100*NODECONFIG.read(EEADDR(io[i].duration));
          else next[i] = 0;
            //PV(next[i]);
        }
      }
    }
  }
}

// ==== Setup does initial configuration ======================
void setup()
{
  #ifdef DEBUG
    Serial.begin(115200); while(!Serial);
    delay(500);
    dP("\n ESP32-2Servo15IO");
  #endif

  NodeID nodeid(NODE_ADDRESS);       // this node's nodeid
  Olcb_init(nodeid, RESET_TO_FACTORY_DEFAULTS);
  servoSetup();
  setupIOPins();
  dP("\n NUM_EVENT="); dP(NUM_EVENT);
}

// ==== Loop ==========================
void loop() {
  bool activity = Olcb_process();
  #ifndef OLCB_NO_BLUE_GOLD
    if (activity) {
      blue.blink(0x1); // blink blue to show that the frame was received
    }
    if (olcbcanTx.active) {
      gold.blink(0x1); // blink gold when a frame sent
      olcbcanTx.active = false;
    }
    // handle the status lights
    gold.process();
    blue.process();
  #endif // OLCB_NO_BLUE_GOLD
  produceFromInputs();  // scans inputs and generates events on change
  appProcess();         // processes io pins, eg flashing
  servoProcess();       // processes servos, moves them to their target
  processProducer();    // processes delayed producer events from inputs
}

