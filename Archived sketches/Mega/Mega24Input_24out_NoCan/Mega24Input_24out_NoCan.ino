/* See README for copywrite and more information on this node.

*/

#include "user_config.h"

#include "processCAN.h"     // Auto-select CAN library
#include "processor.h"      // auto-selects the processor type, EEPROM lib etc.
#include "OpenLCBHeader.h"  // System house-keeping.

// CDI (Configuration Description Information) in XML, must match MemStruct
extern "C" {
#define N(x) xN(x)  // allow the insertion of the value (x) ..
#define xN(x) #x    // .. into the CDI string.
  const char configDefInfo[] PROGMEM =
    // ===== Enter User definitions below =====
    CDIheader R"(
    <name>Application Configuration</name>
    <group replication=')" N(NUM_INPUT) R"('>
        <name>Inputs </name>
        <repname>Pin 14</repname>
        <repname>Pin 15</repname>
        <repname>Pin 16</repname>
        <repname>Pin 17</repname>
        <repname>Pin 18</repname>
        <repname>Pin 19</repname>
        <repname>Pin 20</repname>
        <repname>Pin 21</repname>
        <repname>Pin 22</repname>
        <repname>Pin 23</repname>
        <repname>Pin 24</repname>
        <repname>Pin 25</repname>
        <repname>Pin 26</repname>
        <repname>Pin 27</repname>
        <repname>Pin 28</repname>
        <repname>Pin 29</repname>
        <repname>Pin 30</repname>
        <repname>Pin 31</repname>
        <repname>Pin 32</repname>
        <repname>Pin 33</repname>
        <repname>Pin 34</repname>
        <repname>Pin 35</repname>
        <repname>Pin 36</repname>
        <repname>Pin 37</repname>

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
            </map>
        </int>
        <int size='1'>
            <name>On-delay 1-255 = 100ms-25.5s, 0=none</name>
            <hints><slider tickSpacing='50' immediate='yes' showValue='yes'> </slider></hints>
        </int>
        <int size='1'>
            <name>Off-delay 1-255 = 100ms-25.5s, 0=None</name>
            <hints><slider tickSpacing='50' immediate='yes' showValue='yes'> </slider></hints>
        </int>
        <eventid><name>On-Event</name></eventid>
        <eventid><name>Off-Event</name></eventid>
    </group>
    
    <group replication=')" N(NUM_ACTION_SETS) R"('>
        <name>Outputs </name>
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

        <group replication=')" N(NUM_APS) R"('>
            <name>Actions</name>
            <repname>Action </repname>
            <string size='12'><name>Description of Action on Pins 38-49 A0-A14 </name></string>
            <eventid><name>Event for action</name></eventid>
            <int size="1"><name>pin selection </name>
            <map>
                <relation><property>0</property><value>None</value></relation>
                <relation><property>1</property><value>38</value></relation>
                <relation><property>2</property><value>39</value></relation>
                <relation><property>3</property><value>40</value></relation>
                <relation><property>4</property><value>41</value></relation>
                <relation><property>5</property><value>42</value></relation>
                <relation><property>6</property><value>43</value></relation>
                <relation><property>7</property><value>47</value></relation>
                <relation><property>8</property><value>48</value></relation>
                <relation><property>9</property><value>49</value></relation>
                <relation><property>10</property><value>A0</value></relation>
                <relation><property>11</property><value>A1</value></relation>
                <relation><property>12</property><value>A2</value></relation>
                <relation><property>13</property><value>A3</value></relation>
                <relation><property>14</property><value>A4</value></relation>
                <relation><property>15</property><value>A5</value></relation>
                <relation><property>16</property><value>A6</value></relation>
                <relation><property>17</property><value>A7</value></relation>
                <relation><property>18</property><value>A8</value></relation>
                <relation><property>19</property><value>A9</value></relation>
                <relation><property>20</property><value>A10</value></relation>
                <relation><property>21</property><value>A11</value></relation>
                <relation><property>22</property><value>A12</value></relation>
                <relation><property>23</property><value>A13</value></relation>
                <relation><property>24</property><value>A14</value></relation>
            </map>
            </int>
            <int size="1"><name>action </name>
            <map>
                <relation><property>0</property><value>None</value></relation>
                <relation><property>1</property><value>Low</value></relation>
                <relation><property>2</property><value>High</value></relation>
                <relation><property>3</property><value>Flash</value></relation>
                <relation><property>4</property><value>Double Strobe</value></relation>
                <relation><property>5</property><value>Random</value></relation>               
            </map>
            </int>
            <int size="1"><name>Duration (100 ms)</name></int>
            <int size="1"><name>Rate (100 ms)</name></int>
        </group>
    </group>

    )" CDIfooter;
  // ===== Enter User definitions above =====
}  // end extern

// ===== MemStruct =====
// Memory structure of EEPROM, must match CDI above
typedef struct {
  EVENT_SPACE_HEADER eventSpaceHeader;  // MUST BE AT THE TOP OF STRUCT - DO NOT REMOVE!!!
  char nodeName[20];                    // optional node-name, used by ACDI
  char nodeDesc[24];                    // optional node-description, used by ACDI
  // ===== Enter User definitions below =====
  struct Input {
    char desc[8];
    uint8_t type;
    uint8_t ondelay;  // 100ms-25.5s, 0=steady for On-delay
    uint8_t offdelay;    // 100ms-25.5s, 0=no repeat for Off-delay
    EventID onEid;
    EventID offEid;
  } in[NUM_INPUT];

  struct Action {
    char desc[12];
    EventID eid;
    uint8_t pini;
    uint8_t action;
    uint8_t durn;
    uint8_t rate;
  } action[NUM_ACTION];

  // ===== Enter User definitions above =====
  int8_t currentEvent[NUM_CHANNEL];
} MemStruct;  // type definition

// Pin assignments for Arduino Mega (pins 14-37)
uint8_t inpin[NUM_INPUT] = { 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37 };

uint8_t instate[NUM_INPUT];
bool logstate[NUM_INPUT] = { 0 };  // logic state for toggle
unsigned long next[NUM_INPUT] = { 0 };

uint8_t pin[NUM_CHANNEL] = { 38, 39, 40, 41, 42, 43, 47, 48, 49, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14 };
int8_t currentEvent[NUM_CHANNEL];
uint8_t state[NUM_CHANNEL];
long timer[NUM_CHANNEL];

extern "C" {
// ===== eventid Table =====
#define REG_INPUT(i) PCEID(in[i].onEid), PCEID(in[i].offEid)
#define GROUP(i) CEID(action[i].eid), \
                 CEID(action[i + 1].eid), \
                 CEID(action[i + 2].eid), \
                 CEID(action[i + 3].eid), \
                 CEID(action[i + 4].eid)

//#define REG_PWM(i) PCEID(pwm[i].onEid), PCEID(pwm[i].offEid)

  // Array of the offsets to every eventID in MemStruct/EEPROM/mem, and P/C flags
  const EIDTab eidtab[NUM_EVENT] PROGMEM = {
    REG_INPUT(0), REG_INPUT(1), REG_INPUT(2), REG_INPUT(3), REG_INPUT(4), REG_INPUT(5),
    REG_INPUT(6), REG_INPUT(7), REG_INPUT(8), REG_INPUT(9), REG_INPUT(10), REG_INPUT(11),
    REG_INPUT(12), REG_INPUT(13), REG_INPUT(14), REG_INPUT(15), REG_INPUT(16), REG_INPUT(17),
    REG_INPUT(18), REG_INPUT(19), REG_INPUT(20), REG_INPUT(21), REG_INPUT(22), REG_INPUT(23),
    GROUP(0), GROUP(5), GROUP(10), GROUP(15),
    GROUP(20), GROUP(25), GROUP(30), GROUP(35),
    GROUP(40), GROUP(45), GROUP(50), GROUP(55),
    GROUP(60), GROUP(65), GROUP(70), GROUP(75),
    GROUP(80), GROUP(85), GROUP(90), GROUP(95),

  };


  // SNIP Short node description for use by the Simple Node Information Protocol
  extern const char SNII_const_data[] PROGMEM = "\001" MANU "\000" MODEL "\000" HWVERSION "\000" OlcbCommonVersion;  // last zero in double-quote
}  // end extern "C"

// PIP Protocol Identification Protocol uses a bit-field to indicate which protocols this node supports
uint8_t protocolIdentValue[6] = {
  pSimple | pDatagram | pMemConfig | pPCEvents | !pIdent | pTeach | !pStream | !pReservation,  // 1st byte
  pACDI | pSNIP | pCDI | !pRemote | !pDisplay | !pTraction | !pFunction | !pDCC,               // 2nd byte
  0, 0, 0, 0                                                                                   // remaining 4 bytes
};

#define OLCB_NO_BLUE_GOLD  // blue/gold not used in this sketch

// Initialize EEPROM to Factory Defaults

bool initialized = false;
void userInitAll() {
  NODECONFIG.put(EEADDR(nodeName), ESTRING("Mega"));
  NODECONFIG.put(EEADDR(nodeDesc), ESTRING("24in 24out"));
  for (uint8_t i = 0; i < NUM_INPUT; i++) {
    NODECONFIG.put(EEADDR(in[i].desc), ESTRING(""));
    NODECONFIG.update(EEADDR(in[i].type), 0);
    NODECONFIG.update(EEADDR(in[i].ondelay), 0);
    NODECONFIG.update(EEADDR(in[i].offdelay), 0);
    instate[i] = 0xFF;
  }
  for (uint8_t i = 0; i < NUM_ACTION; i++) {
    NODECONFIG.put(EEADDR(action[i].desc), ESTRING(""));
    NODECONFIG.update(EEADDR(action[i].pini), 0);
    NODECONFIG.update(EEADDR(action[i].action), 0);
    NODECONFIG.update(EEADDR(action[i].durn), 0);
    NODECONFIG.update(EEADDR(action[i].rate), 0);
  }
  for (uint8_t i = 0; i < NUM_CHANNEL; i++) {
    NODECONFIG.update(EEADDR(currentEvent[i]), -1);
  }
}

// Determine the state of each eventid
enum evStates { VALID = 4,
                INVALID = 5,
                UNKNOWN = 7 };
uint8_t userState(uint16_t index) {
  if(index<(NUM_INPUT*2)) {
    uint8_t ch = index / 2;
    uint8_t type = NODECONFIG.read(EEADDR(in[ch].type));
    if (type == 0) return UNKNOWN;
    bool s = digitalRead(inpin[ch]);
    if (!s ^ (type & 1)) return VALID;
    else return INVALID;
  } 
  uint8_t ch = index - NUM_INPUT*2;  // get ch for outouts
  uint8_t action = NODECONFIG.read(EEADDR(action[ch].action));
  if (action == 0) return UNKNOWN;
  if (!state[ch] ^ (action & 1)) return VALID;
  else return INVALID;
}

#define PV(x) { dP(" " #x "="); dP(x); }
#define PVH(x) { dP(" " #x "="); Serial.print(x, HEX); }
#define PVL(x) { dP("\n" #x "="); dP(x); }

// Process Consumer-eventIDs (no action for inputs-only)
void pceCallback(uint16_t index) {
  dP("\npceCallback("); dP(index);
  if( index < NUM_INPUT*2 ) return; // inouts do not consume
  index -= NUM_INPUT*2; // adjust index for actions.  
  PVL(index);
  PVL(NODECONFIG.read(EEADDR(action[index].pini))); 
  PVL(NODECONFIG.read(EEADDR(action[index].action))); 
  PVL(NODECONFIG.read(EEADDR(action[index].durn))); 
  PVL(NODECONFIG.read(EEADDR(action[index].rate)));
  uint8_t i = NODECONFIG.read(EEADDR(action[index].pini)) - 1;  // correct to 0-index
  currentEvent[i] = index;
  timer[i] = millis() + NODECONFIG.read(EEADDR(action[index].durn));
  state[i] = 0;
  PVL(i); PV(currentEvent[i]); PV(timer[i]); PV(state[i]);
}

// Process Inputs
void produceFromInputs() {
  const uint8_t base = 0;
  static uint8_t c = 0;
  static unsigned long last = 0;
  if ((millis() - last) < (50 / NUM_INPUT)) return;
  last = millis();
  uint8_t type = NODECONFIG.read(EEADDR(in[c].type));
  uint8_t d;
  if (type == 5 || type == 6) {
    bool s = digitalRead(inpin[c]);
    if (s != instate[c]) {
      instate[c] = s;
      if (!s) {
        logstate[c] ^= 1;
        if (logstate[c]) d = NODECONFIG.read(EEADDR(in[c].ondelay));
        else d = NODECONFIG.read(EEADDR(in[c].offdelay));
        if (d == 0) OpenLcb.produce(base + c * 2 + logstate[c]);  // no delay, send event
        else next[c] = millis() + (uint16_t)d * 100;              // register delay
      }
    }
  }
  if (type > 0 && type < 5) {
    bool s = digitalRead(inpin[c]);
    if (s != instate[c]) {
      instate[c] = s;
      if (!instate[c]) d = NODECONFIG.read(EEADDR(in[c].ondelay));
      else d = NODECONFIG.read(EEADDR(in[c].offdelay));
      if (d == 0) OpenLcb.produce(base + c * 2 + (!s ^ (type & 1)));  // no delay, send event
      else next[c] = millis() + (uint16_t)d * 100;                    // register delay
    }
  }
  if (++c >= NUM_INPUT) c = 0;
}

// Process pending producer events
void processProducer() {
  const uint8_t base = 0;
  static unsigned long last = 0;
  unsigned long now = millis();
  if ((now - last) < 50) return;
  for (int c = 0; c < NUM_INPUT; c++) {
    if (next[c] == 0) continue;
    if (now < next[c]) continue;
    uint8_t type = NODECONFIG.read(EEADDR(in[c].type));
    if (type > 6) continue;  // no outputs to process
    uint8_t s = instate[c];
    if (type < 5) OpenLcb.produce(base + c * 2 + (!s ^ (type & 1)));  // regular inputs
    else OpenLcb.produce(base + c * 2 + logstate[c]);                 // toggle inputs
    next[c] = 0;
  }
}

// Setup the input pins
void setupPins() {
  dP("\nPins: ");
  for (uint8_t i = 0; i < NUM_INPUT; i++) {
    uint8_t type = NODECONFIG.read(EEADDR(in[i].type));
    switch (type) {
      case 1:
      case 2:
      case 5:
        dP(" IN:");
        pinMode(inpin[i], INPUT);
        instate[i] = 0xFF;  // trigger first send
        break;
      case 3:
      case 4:
      case 6:
        dP(" INP:");
        pinMode(inpin[i], INPUT_PULLUP);
        instate[i] = 0xFF;
        break;
      default:
        dP(" NONE:");
        pinMode(inpin[i], INPUT);  // default to input for safety
        instate[i] = 0xFF;
        break;
    }
    dP(inpin[i]);
    dP(":");
    dP(type);
    dP(", ");
  }
  for(uint8_t i=0; i<NUM_CHANNEL; i++) {
    pinMode( pin[i], OUTPUT );
  }
}

// Callback from a Configuration write
void userConfigWritten(uint32_t address, uint16_t length, uint16_t func) {
  dPS("\nuserConfigWritten: Addr: ", (uint32_t)address);
  dPS(" Len: ", (uint16_t)length);
  dPS(" Func: ", (uint8_t)func);
  setupPins();
}

#include "OpenLCBMid.h"           // System house-keeping

enum Action { aNONE=0, aLOW=1, aHIGH, aFLASH, aDSTROBE, aRANDOM, aFIRE, aFIREOFF };

void doAction(uint8_t i, uint8_t pi, uint8_t actn, uint8_t durn, uint8_t rate) {
  //PVL(pi); PV(pin[pi]); PV(actn); PV(durn); PV(rate); PV(state[i]); 
  long now = millis();
  bool inv = false;
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
    //case aFLASHB: inv=true;
    case aFLASH:
      if( timer[i] && (now-timer[i])>0 ) {
        //dP("\naFLASH"); 
        if( state[i]==0 ) {
          digitalWrite(pin[pi], HIGH);
          //digitalWrite(pin[pi], !inv);
          timer[i] = now+durn*100;
          state[i] = 1;
        } else {
          digitalWrite(pin[pi], LOW);
          //digitalWrite(pin[pi], inv);
          timer[i] = now+rate*100;
          state[i] = 0;
        }
      }
      break;
    //case aDSTROBEI: inv=true;
    case aDSTROBE:
      if( timer[i] && (now-timer[i])>0 ) {
        //dP("\naDSTROBE"); PV(state[i]);
        switch( state[i] ) {
          case 0:
            digitalWrite(pin[pi], HIGH);
            //digitalWrite(pin[pi], !inv);
            timer[i] = now+100;
            state[i] = 1;
            break;
          case 1:
            digitalWrite(pin[pi], LOW);
            //digitalWrite(pin[pi], inv);
            timer[i] = now+durn*100;
            state[i] = 2;
            break;
          case 2:
            digitalWrite(pin[pi], HIGH);
            //digitalWrite(pin[pi], !inv);
            timer[i] = now+100;
            state[i] = 3;
            break;
          case 3:
            digitalWrite(pin[pi], LOW);
            //digitalWrite(pin[pi], inv);
            timer[i] = now+rate*100;
            state[i] = 0;
            break;
          default:
            state[i] = 0;
        }
      }
      break;
    case aRANDOM:
      if( timer[i] && (now-timer[i])>0 ) {
        //dP("\naRANDOM"); PV(state[i]);
        switch( state[i] ) {
          case 0:
            digitalWrite(pin[pi], HIGH);
            timer[i] = now+random(durn*durn*5, durn*durn*20);
            //PV(timer[i]);
            state[i] = 1;
            break;
          case 1:
            digitalWrite(pin[pi], LOW);
            timer[i] = now+random(rate*rate*5, rate*rate*20);
            //PV(timer[i]);
            state[i] = 0;
            break;
          default: 
            state[i] = 0;
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
        //dP("\naFIRE"); dP(pin[pi]);
        if(pi+1>NUM_CHANNEL) break;
        analogWrite(pin[pi], random(durn));
        analogWrite(pin[pi+1], random(rate));
        timer[i] = now+15;
      }
      break;
  }
}

void doActions() {
  static uint8_t i = 255;
  uint8_t pi, actn, rate, durn; 
  //static long last = 0;
  //if( (now-last) <(50/NUM_CHANNEL) ) return;
  //last = now;
  if(!initialized) return;
  if(++i>=NUM_CHANNEL) i=0;
  int8_t ai = currentEvent[i];
  if(ai==-1) return;
  //PVL(i); PV(ai);
  pi = NODECONFIG.read(EEADDR(action[ai].pini))-1;  // pin number
  actn = NODECONFIG.read(EEADDR(action[ai].action));
  durn = NODECONFIG.read(EEADDR(action[ai].durn));
  rate = NODECONFIG.read(EEADDR(action[ai].rate));
  //PVL(i); PV(ai); PV(pi); PV(actn); PV(durn); PV(rate); 
  doAction(i, pi, actn, durn, rate);
}

void userSoftReset() {
  //dP("\n In userSoftReset()"); Serial.flush();
  REBOOT;  // defined in processor.h for each mpu
}

void userHardReset() {
  //dP("\n In userHardReset()"); Serial.flush();
  REBOOT;  // defined in processor.h for each mpu
}



// Setup does initial configuration
void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial) delay(50); 
  dP("\n 24In24out");
  dP("\n" __FILE__);
#endif

  NodeID nodeid(NODE_ADDRESS);  // this node's nodeid
  Olcb_init(nodeid, RESET_TO_FACTORY_DEFAULTS);
  setupPins();
  dP("\n NUM_EVENT=");
  dP(NUM_EVENT);

  // retrieve current-events from eeprom
  for(int c=0; c<NUM_CHANNEL; c++) {
    currentEvent[c] = NODECONFIG.read( EEADDR( currentEvent[c] ) );
  }

#if 1  // test values
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
    

  #endif

  initialized = true;
}

void loop() {
  bool activity = Olcb_process();
  produceFromInputs();  // scans inputs and generates events on change
  processProducer();    // processes delayed producer events from inputs
  doActions();
}

void userUI(char c) {
  switch (c) {
    case 'A':      // Actions:
      dP("\nActions:");
      for(int i=0; i<NUM_ACTION; i++) {
        uint8_t pini = NODECONFIG.read( EEADDR( action[i].pini ) );
        if(pini==0) continue;
        uint8_t action = NODECONFIG.read( EEADDR( action[i].action ) );
        uint8_t durn = NODECONFIG.read( EEADDR( action[i].durn ) );
        uint8_t rate = NODECONFIG.read( EEADDR( action[i].rate ) );
        PVL(i); PV(pini); PV(pin[pini-1]); PV(action); PV(durn); PV(rate);
      }
      break;
    case 'C':    // Current Events:
      dP("\nCurrent Events:");
      for(int i=0; i<NUM_CHANNEL; i++) {
        if(currentEvent[i]==-1) continue;
        uint8_t ai = currentEvent[i];
        uint8_t pini = NODECONFIG.read( EEADDR( action[ai].pini ) );
        uint8_t action = NODECONFIG.read( EEADDR( action[ai].action ) );
        uint8_t durn = NODECONFIG.read( EEADDR( action[ai].durn ) );
        uint8_t rate = NODECONFIG.read( EEADDR( action[ai].rate ) );
        PVL(i); PV(pini); PV(pin[pini]); PV(action); PV(durn); PV(rate);
      }      
      break;
    case 'Z':   // clear current-events
      for(int i=0; i<NUM_CHANNEL; i++) currentEvent[i] = -1;
      break;
    case '0' ... '6':
      {
        uint8_t index = c-'0';
        uint8_t i = NODECONFIG.read(EEADDR(action[index].pini)) - 1;  // correct to 0-index
        currentEvent[i] = index;
        timer[i] = millis() + NODECONFIG.read(EEADDR(action[index].durn));
        state[i] = 0;
      }
      dP("\nMessage needed to activate this effect: "); dP(" type: :X195B4123N050101018E0700"); Serial.print(0x30+c-'0', HEX); dP(";");
      break;
    case '?': 
    default:
      dP("\nHelp: Actions; Current-events; Zero currentEvents; 0-6: activate action0-action6.");
      break;
  }
}