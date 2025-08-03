
/*This is in beta testing but here to give a chance to have a look
  at the sketch

  2025.08.25 changes: Can Transceiver Version Only
  Use update and update16 instead of write to reduce EEPROM wear
  Moved initialization of curpos (may have been overwriting things!)

This is my test version for demonstration CAN Bus use only by John Holmes
  - Pins 19 RX and 18 TX for the transceiver module
  - Pins 14,27,26,32,15,4,16,23  are used for input or output
  - Pins 25,33 servos
  - Pin  21 SDA 
  - Pin  22 SCL

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

#include "Config.h"   // Contains configuration, see "Config.h"

#include "mdebugging.h"           // debugging
//#include "processCAN.h"           // Auto-select CAN library
//#include "processor.h"            // auto-selects the processor type, EEPROM lib etc.
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
    <hints><visibility hideable='yes' hidden='yes' ></visibility></hints>
    <group>
      <name>Turnout Servo Configuration</name>
      <hints><visibility hideable='yes' hidden='yes' ></visibility></hints>
      <int size='1'>
        <name>Speed 5-50 (delay between steps)</name>
        <min>5</min><max>50</max>
        <hints><slider tickSpacing='15' immediate='yes' showValue='yes'> </slider></hints>
      </int>
      <int size='1'><name>Save servo positions every x*x= seconds</name></int>
    </group>
    <group replication=')" N(NUM_SERVOS) R"('>
      <name>Servos</name>
      <hints><visibility hideable='yes' hidden='yes' ></visibility></hints>
      <repname>Servo </repname>
      <string size='8'><name>Description</name></string>
      <group replication=')" N(NUM_POS) R"('>
      <name>  Closed     Midpoint   Thrown</name>
        <repname>Position</repname>
        <eventid><name>EventID</name></eventid>
        <int size='1'>
          <name>Servo Position in Degrees</name>
          <min>0</min><max>180</max>
          <hints><slider tickSpacing='45' immediate='yes' showValue='yes'> </slider></hints>
        </int>
      </group>
    </group>
    <group replication=')" N(NUM_NATIVE_IO) R"('>
      <name>Native IO</name>
      <hints><visibility hideable='yes' hidden='yes' ></visibility></hints>
      <repname>1</repname>
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
    <group replication=')" N(NUM_MCP_GROUPS) R"('>
      <name>MCP23017 IO</name>
      <hints><visibility hideable='yes' hidden='yes' ></visibility></hints>
      <repname>Port A</repname>
      <repname>Port B</repname>
      <group replication=')" N(NUM_MCP_IO_PER_GROUP) R"('>
        <name>Port IO</name>
        <repname>0</repname>
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
          uint8_t saveperiod; // period in seconds to save the servo positions
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
    #define REG_NAT(g) REG_IO(g+0), REG_IO(g+1), REG_IO(g+2), REG_IO(g+3), REG_IO(g+4), REG_IO(g+5), REG_IO(g+6), REG_IO(g+7) 
    
    //  Array of the offsets to every eventID in MemStruct/EEPROM/mem, and P/C flags
    const EIDTab eidtab[NUM_EVENT] PROGMEM = {
        //REG_SERVO_OUTPUT(0), REG_SERVO_OUTPUT(1),
        //REG_NAT(0), REG_NAT(8), REG_NAT(16), 
        SERVOEID(NUM_SERVOS),
        IOEID(NUM_IO)
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

//#include <Servo.h>    //// NANO, Minima etc
//#include <ESP32Servo.h> //// ESP32
Servo servo[2];
uint8_t servodelay;
//uint8_t servopin[NUM_SERVOS] = {25,33};  // CHOOSE PINS FOR SERVOS
uint8_t servopin[NUM_SERVOS] = { SERVOPINS };  // CHOOSE PINS FOR SERVOS
uint8_t servoActual[NUM_SERVOS];
uint8_t servoTarget[NUM_SERVOS];

uint8_t iopin[NUM_IO] = { IOPINS }; //// ESP32 

enum Type { tNONE=0, tIN, tINI, tINP, tINPI, tTOG, tTOGI, tPA, tPAI, tPB, tPBI };
bool iostate[NUM_IO] = {0};  // state of the iopin
bool logstate[NUM_IO] = {0}; // logic state for toggle
unsigned long next[NUM_IO] = {0};

// This is called to initialize the EEPROM to Factory Reset
void userInitAll()
{ 
  NODECONFIG.put(EEADDR(nodeName), ESTRING(BOARD));
  NODECONFIG.put(EEADDR(nodeDesc), ESTRING("2Servos24IO"));
  NODECONFIG.update(EEADDR(servodelay), 50);
  NODECONFIG.update(EEADDR(saveperiod), 50);
  for(uint8_t i = 0; i < NUM_SERVOS; i++) {
    NODECONFIG.put(EEADDR(servos[i].desc), ESTRING(""));
    for(int p=0; p<NUM_POS; p++) {
      NODECONFIG.update(EEADDR(servos[i].pos[p].angle), 90);
      NODECONFIG.update(EEADDR(curpos[i]), 0);
    }
  }
  dP("\n NUM_IO"); dP(NUM_IO);
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
    }
    int ch = (index - NUM_SERVOS*NUM_POS)/2;
    if( NODECONFIG.read(EEADDR(io[ch].type))==0) return UNKNOWN;
    int evst = index % 2;
    //dP((uint8_t) iostate[ch]==evst);
    if( iostate[ch]==evst ) return VALID;
    return INVALID;
}
    

  #ifdef DEBUG
    #define PV(x) { dP(" " #x "="); dP(x); }
  #else
    #define PV(x) 
  #endif

// ===== Process Consumer-eventIDs =====
void pceCallback(uint16_t index) {
// Invoked when an event is consumed; drive pins as needed
// from index of all events.
// Sample code uses inverse of low bit of pattern to drive pin all on or all off.
// The pattern is mostly one way, blinking the other, hence inverse.
//
  dP("\npceCallback, index="); dP((uint16_t)index);
  if(index<NUM_SERVOS*NUM_POS) { dP("\n servo "); dP(index/2); }
  else if(index<(NUM_SERVOS*NUM_POS+NUM_NATIVE_IO*2)) { dP("\n native "); dP((index-NUM_SERVOS*NUM_POS)/2); }
  else { dP("\n mcp "); dP((index-NUM_SERVOS*NUM_POS-NUM_NATIVE_IO*2)/2); }
    if(index<NUM_SERVOS*NUM_POS) {
      uint8_t outputIndex = index / 3;
      uint8_t outputState = index % 3;
      curpos[outputIndex] = outputState;
      servo[outputIndex].attach(servopin[outputIndex]);
      servoTarget[outputIndex] = NODECONFIG.read( EEADDR(servos[outputIndex].pos[outputState].angle) );
    } else {
      int ch = (index - NUM_SERVOS*NUM_POS)/2;
      PV(ch);
      uint8_t type = NODECONFIG.read(EEADDR(io[ch].type));
      PV(type);
      if(type>=7) {
        // 7=PA 8=PAI 9=PB 10=PBI
        bool inv = !(type&1);       // inverted
        if( index%2 ) {
          dP("\noff"); PV(ch); PV(type); PV(inv);
          if(ch<NUM_NATIVE_IO) digitalWrite( iopin[ch], inv);
          else mcp.digitalWrite( ch-NUM_NATIVE_IO, inv);
          next[ch] = 0;
        } else {
          bool pha = type<9;       // phaseA
          dP("\nON"); PV(ch); PV(pha); PV(inv); 
          if(ch<NUM_NATIVE_IO) digitalWrite( iopin[ch], pha ^ inv);
          else mcp.digitalWrite( ch-NUM_NATIVE_IO, pha ^ inv);
          iostate[ch] = 1;
          uint8_t durn;
          durn = NODECONFIG.read(EEADDR(io[ch].duration));
          PV(durn);
          if(durn) next[ch] = millis() + 100*durn; // note duration==0 means forever
          else next[ch]=0;
          PV(millis()); PV(next[ch]);
        }
      }
    }
}

// === Process servos ===
// This is called from loop to service the servos
bool posdirty = false;
void servoProcess() {
  static long last = 0;
  if( (millis()-last) < servodelay ) return;
  last = millis();
  static long lastmove = 0;
  for(int i=0; i<NUM_SERVOS; i++) {
    if(servoTarget[i] == servoActual[i] ) continue;
    if(servoTarget[i] > servoActual[i] ) servoActual[i]++;
    else if(servoTarget[i] < servoActual[i] ) servoActual[i]--;
    if(!servo[i].attached()) { 
      servo[i].attach(servopin[i]);
      delay(100);
    }
    //servo[i].attach(servopin[i]);
    servo[i].write(servoActual[i]);
    //P("\n servomove"); PV(i); PV(servoTarget[i]); PV(servoActual[i]);
    lastmove = millis();
    posdirty =true;
  }

  if( lastmove && (millis()-lastmove)>4000) {
    for(int i=0; i<NUM_SERVOS; i++) servo[i].detach();
    lastmove = 0;
    //dP("\n detach()");
    //printMem();
  }

  static long lastsave = 0;
  uint32_t saveperiod = getSavePeriod();
  if(saveperiod && posdirty && (millis()-lastsave) > saveperiod ) {
    lastsave = millis();
    posdirty = false;
    //for(int i=0; i<NUM_SERVOS; i++) 
    //  NODECONFIG.update( EEADDR(curpos[i]), curpos[i]);
    for(int i=0; i<NUM_SERVOS; i++) NODECONFIG.update( EEADDR(curpos[i]), curpos[i]);
    EEPROMcommit;
    //dP("\n save curpos\n curpos[0]="); dP(NODECONFIG.read( EEADDR(curpos[0])));
    //dP("\n curpos[1]="); dP(NODECONFIG.read( EEADDR(curpos[1])));
  }
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
      //dP("\n"); PV(c); PV(type);
      bool s;
      if(c<NUM_NATIVE_IO) s = digitalRead(iopin[c]);
      else s = mcp.digitalRead(c-NUM_NATIVE_IO);
      //PV(s); PV(iostate[c]);
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
      //dP("\n"); PV(c); PV(type);
      bool s;
      if(c<NUM_NATIVE_IO) s = digitalRead(iopin[c]);
      else s = mcp.digitalRead(c-NUM_NATIVE_IO);
      if(s != iostate[c]) {
        iostate[c] = s;
        d = NODECONFIG.read(EEADDR(io[c].duration)); 
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
    if(now<next[c]) continue; 
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
// Use this to detect changes in the node's configuration
// This may be useful to take immediate action on a change.
void userConfigWritten(uint32_t address, uint16_t length, uint16_t func)
{
  dPS("\nuserConfigWritten: Addr: ", (uint32_t)address);
  dPS(" Len: ", (uint16_t)length);
  dPS(" Func: ", (uint8_t)func);
  setupIOPins();
  servodelay = NODECONFIG.read( EEADDR( servodelay ) );
  servoSet();
}

// retrieve savePeriod
uint32_t getSavePeriod() {
  uint32_t saveperiod = NODECONFIG.read( EEADDR(saveperiod) );
  return saveperiod * saveperiod * 1000;
}

// On startup: set curpos[i] to 1 and set servo to 90 or saved angle
void servoStartUp() {
  dP("\n servoStartUp");
  servodelay = NODECONFIG.read( EEADDR( servodelay ) );
  for(int i=0; i<NUM_SERVOS; i++) {
    if(getSavePeriod()==0) curpos[i] = 1;                    // if positions are not saved, then default to middle position
    else curpos[i] = NODECONFIG.read( EEADDR( curpos[i] ) ); // else retrieve the last postion
    dP("\n i="); dP(i); dP(" curpos[i]="); dP(curpos[i]);
    if( USE_90_ON_STARTUP ) servoActual[i] = 90;                      // if to use 90degrees, then set its Actual postion to 90
    else servoActual[i] = 
         NODECONFIG.read( EEADDR( servos[i].pos[curpos[i]].angle ) ); // else retrieve the angle of the set position
    dP(" servoActual[i]="); dP(servoActual[i]);
    servo[i].write(servoActual[i]);
    delay(100);
    servo[i].attach(servopin[i]);
    delay(100);
    servo[i].write(servoActual[i]);  // put the servo to its Actual position.  
    dP(" done");
  }
  servoSet();
}
// Allow Servo adjustments
void servoSet() {
  for(int i=0; i<NUM_SERVOS; i++) {
    servoTarget[i] = NODECONFIG.read( EEADDR( servos[i].pos[curpos[i]].angle ) );
  }
}

// Setup the io pins
// called by setup() and after a configuration change
void setupIOPins() {
  dP("\nPins: ");
  for(uint8_t i=0; i<NUM_IO; i++) {
    uint8_t type = NODECONFIG.read( EEADDR(io[i].type));
    switch (type) {  // No PULLUP
      case 1: case 2: case 5:
        if(type==1) dP(" IN");
        if(type==2) dP(" INI");
        if(type==5) dP(" TOG");
        if(i<NUM_NATIVE_IO) pinMode(iopin[i], INPUT);
        else mcp.pinMode(i-NUM_NATIVE_IO, INPUT);
        iostate[i] = type&1;
        if(type==5) iostate[i] = 0;
        break;
      case 3: case 4: case 6: // PULLUPS
        if(type==3) dP(" INP");
        if(type==4) dP(" INPI");
        if(type==6) dP(" TOGP");
        if(i<NUM_NATIVE_IO) pinMode(iopin[i], INPUT_PULLUP); 
        else mcp.pinMode(i-NUM_NATIVE_IO, INPUT_PULLUP);
        iostate[i] = type&1;
        break;
      case 7: case 8: case 9: case 10:
        if(type==7) dP(" PA");
        if(type==8) dP(" PAI");
        if(type==9) dP(" PB");
        if(type==10) dP(" PBI");
        if(i<NUM_NATIVE_IO) pinMode(iopin[i], OUTPUT); 
        else mcp.pinMode(i-NUM_NATIVE_IO, OUTPUT); 
        iostate[i] = !type&1;
        if(i<NUM_NATIVE_IO) digitalWrite(iopin[i], !type&1);
        else mcp.digitalWrite(i-NUM_NATIVE_IO, !type&1);
        break;
    }
    if(i<NUM_NATIVE_IO) { dP("\n native "); dP(iopin[i]); dP(":"); dP(type); }
    else { dP("\n mcp "); dP(i-NUM_NATIVE_IO); dP(":"); dP(type); }
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
          dP("\nphaseB"); PV(i); PV(phb); PV(inv); PV(phb ^ inv);
          if(i<NUM_NATIVE_IO) {
            digitalWrite(iopin[i], phb ^ inv);
            PV(iopin[i]);
          } else {
            mcp.digitalWrite(i-NUM_NATIVE_IO, phb ^ inv);
            PV(i-NUM_NATIVE_IO)
          }
          iostate[i] = 0;
          if( NODECONFIG.read(EEADDR(io[i].period)) > 0 ) 
          next[i] = now + 100*NODECONFIG.read(EEADDR(io[i].period));
          else next[i] = 0;
            PV(next[i]);
        } else {
          // phaseA
          dP("\nphaseA"); PV(i); PV(phb); PV(inv); PV(!phb ^ inv);
          if(i<NUM_NATIVE_IO) digitalWrite(iopin[i], !phb ^ inv);
          else mcp.digitalWrite(i-NUM_NATIVE_IO, !phb ^ inv);
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
    dP("\n AVR-2Servo8IOCAN");
    dP("\n num servos = "); dP(NUM_SERVOS);
    dP("\n num servo pos = "); dP(NUM_POS);
    dP("\n num native io = "); dP(NUM_NATIVE_IO);
    dP("\n num mcp groups = "); dP(NUM_MCP_GROUPS);
    dP("\n num io in each mcp group"); dP(NUM_MCP_IO_PER_GROUP);
    dP("\n num mcp io = "); dP(NUM_MCP_IO);
    dP("\n total num io = "); dP(NUM_IO);
    dP("\n num events = "); dP(NUM_EVENT);
  #endif

  WIRE_begin;                        // defined in boards.h
  EEPROMbegin;
  NodeID nodeid(NODE_ADDRESS);       // this node's nodeid
  Olcb_init(nodeid, RESET_TO_FACTORY_DEFAULTS);
  setupIOPins();
  servoStartUp();
  dP("\n NUM_EVENT="); dP(NUM_EVENT);
  
//   #if 0
//   for(uint8_t i=0; i<NUM_IO; i++) {
//     NODECONFIG.write( EEADDR( io[i].type ), tNONE);
//     NODECONFIG.write( EEADDR( io[i].duration ), 0);
//     NODECONFIG.write( EEADDR( io[i].period ), 0);
//   }

//   NODECONFIG.write( EEADDR( io[16+2].type ), tPA);
//   NODECONFIG.write( EEADDR( io[16+2].duration ), 5);
//   NODECONFIG.write( EEADDR( io[16+2].period ), 5);

//   NODECONFIG.write( EEADDR( io[16+3].type ), tPB);
//   NODECONFIG.write( EEADDR( io[16+3].duration ), 5);
//   NODECONFIG.write( EEADDR( io[16+3].period ), 5);

//   NODECONFIG.write( EEADDR( io[16+4].type ), tIN);
//   NODECONFIG.write( EEADDR( io[16+4].duration ), 10);
//   NODECONFIG.write( EEADDR( io[16+4].period ), 10);
//   #endif

//     for(int i=0; i<NUM_IO; i++) {
//       dP("\n"); dP(i); 
//       dP(" type="); dP( NODECONFIG.read( EEADDR( io[i].type ) ) ); 
//     }

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

