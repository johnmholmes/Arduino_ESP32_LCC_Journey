/*This is in beta testing but here to give a chance to have a look
  at the sketch

This is my test version for demonstration CAN Bus use only by John Holmes
  - Pins 15 RX and 2 TX for the transceiver module will be my default in future.
  - Pins 16,17,18,19,14,27,26,25  are used for input or output
  - Pins 32,33 servos
  - Pin  21 SDA 
  - Pin  22 SCL

*/
//==============================================================
// AVR 2Servos 24 IO using ESPcan
//
// Coprright 2024 David P Harris
// derived from work by Alex Shepherd and David Harris
// Updated 2026.June.03 by John Holmes
//==============================================================
// - 2 Servo channels, each with 
//     - three settable positions
//     - three set position events 
//     - two midpoint events
//     - Two end target reached vents
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

#include "Config.h"
#include "mdebugging.h"
#include "OpenLCBHeader.h"

// ==== New: centralize per-servo event mapping ====
// CDI defines per-servo events:
// 0..2  = Position consumers (Closed, Mid, Thrown)
// 3     = Frog To Thrown (producer, midpoint when heading to Thrown)
// 4     = Frog To Closed (producer, midpoint when heading to Closed)
// 5     = Reached Thrown (producer)
// 6     = Reached Closed (producer)
enum {
  EVT_POS_CLOSED = 0,
  EVT_POS_MID    = 1,
  EVT_POS_THROWN = 2,
  EVT_FROG_TO_THROWN = 3,
  EVT_FROG_TO_CLOSED = 4,
  EVT_REACHED_THROWN = 5,
  EVT_REACHED_CLOSED = 6
};
constexpr uint8_t SERVO_EVENT_STRIDE = (NUM_POS + 4); // 7
constexpr uint16_t SERVO_EVENTS_TOTAL = (NUM_SERVOS * SERVO_EVENT_STRIDE);
constexpr uint16_t IO_EVENTS_BASE = SERVO_EVENTS_TOTAL;

extern "C" {
  #define N(x) xN(x)
  #define xN(x) #x
  const char configDefInfo[] PROGMEM = 

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
      <eventid><name>frogToThrownEID This eventid is produced when passing the midpoint heading towards Thrown</name></eventid>
      <eventid><name>frogToClosedEID This eventid is produced when passing the midpoint heading towards Closed</name></eventid>
      <eventid><name>Servo has reached its Thrown target position</name></eventid>
      <eventid><name>Servo has reached its Closed target position</name></eventid>
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
        <repname>1</repname>
        <repname>2</repname>
        <repname>3</repname>
        <repname>4</repname>
        <repname>5</repname>
        <repname>6</repname>
        <repname>7(OUT ONLY)</repname>
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
            <relation><property>7</property><value>Output Phase A</value></relation>
            <relation><property>8</property><value>Output Phase A Inverted</value></relation>
            <relation><property>9</property><value>Output Phase B</value></relation>
            <relation><property>10</property><value>Output Phase B Inverted</value></relation>
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
}

// ===== MemStruct (unchanged except comments) =====
typedef struct {
  EVENT_SPACE_HEADER eventSpaceHeader;
  char nodeName[20];
  char nodeDesc[24];
  uint8_t servodelay;
  uint8_t saveperiod;
  struct {
    char desc[8];
    struct {
      EventID eid;
      uint8_t angle;
    } pos[NUM_POS];
    EventID frogToThrownEID; // producer
    EventID frogToClosedEID; // producer
    EventID ThrownTargetEID; // producer
    EventID ClosedTargetEID; // producer
  } servos[NUM_SERVOS];
  struct {
    char desc[8];
    uint8_t type;
    uint8_t duration;
    uint8_t period;
    EventID onEid;
    EventID offEid;
  } io[NUM_IO];
  uint8_t curpos[NUM_SERVOS];
} MemStruct;

uint8_t curpos[NUM_SERVOS];

extern "C" {
  const EIDTab eidtab[NUM_EVENT] PROGMEM = {
    SERVOEID(NUM_SERVOS),
    IOEID(NUM_IO)
  };
  extern const char SNII_const_data[] PROGMEM = "\001" MANU "\000" MODEL "\000" HWVERSION "\000" OlcbCommonVersion ;
}

uint8_t protocolIdentValue[6] = {
  pSimple | pDatagram | pMemConfig | pPCEvents | !pIdent    | pTeach     | !pStream   | !pReservation,
  pACDI   | pSNIP     | pCDI       | !pRemote  | !pDisplay  | !pTraction | !pFunction | !pDCC,
  0, 0, 0, 0
};

#define OLCB_NO_BLUE_GOLD

// ==== Servo and IO arrays ====
// Make servo array match NUM_SERVOS
Servo servo[NUM_SERVOS];
uint8_t servodelay;
uint8_t servopin[NUM_SERVOS] = { SERVOPINS };
uint8_t servoActual[NUM_SERVOS];
uint8_t servoTarget[NUM_SERVOS];

uint8_t iopin[NUM_IO] = { IOPINS };

enum Type { tNONE=0, tIN, tINI, tINP, tINPI, tTOG, tTOGI, tPA, tPAI, tPB, tPBI };
bool iostate[NUM_IO] = {0};
bool logstate[NUM_IO] = {0};
unsigned long next[NUM_IO] = {0};

#ifdef DEBUG
  #define PV(x) { Serial.print(" " #x "="); Serial.print(x); }
#else
  #define PV(x)
#endif

void userInitAll()
{
  NODECONFIG.put(EEADDR(nodeName), ESTRING(BOARD));
  NODECONFIG.put(EEADDR(nodeDesc), ESTRING("G3_2Servos24IO"));
  NODECONFIG.update(EEADDR(servodelay), 50);
  NODECONFIG.update(EEADDR(saveperiod), 50);
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

enum evStates { VALID=4, INVALID=5, UNKNOWN=7 };

// ==== Fixed: userState uses stride and does not early-return before frog/reached ====
uint8_t userState(uint16_t index) {
  if (index < SERVO_EVENTS_TOTAL) {
    uint8_t ch = index / SERVO_EVENT_STRIDE;
    uint8_t ev = index % SERVO_EVENT_STRIDE;

    if (ev < NUM_POS) {
      return (curpos[ch] == ev) ? VALID : INVALID;
    }
    if (ev == EVT_FROG_TO_THROWN) {
      if (curpos[ch] == 0) return VALID;
      if (curpos[ch] == 1) return UNKNOWN;
      return INVALID;
    }
    if (ev == EVT_FROG_TO_CLOSED) {
      if (curpos[ch] == 2) return VALID;
      if (curpos[ch] == 1) return UNKNOWN;
      return INVALID;
    }
    if (ev == EVT_REACHED_THROWN) return (curpos[ch] == 2) ? VALID : INVALID;
    if (ev == EVT_REACHED_CLOSED) return (curpos[ch] == 0) ? VALID : INVALID;

    return UNKNOWN;
  }

  // IO states (2 events per IO channel)
  index -= SERVO_EVENTS_TOTAL;
  int ch = index / 2;
  if (NODECONFIG.read(EEADDR(io[ch].type)) == 0) return UNKNOWN;
  uint8_t eidstate = (index % 2) ? 1 : 0; // even = ON, odd = OFF
  if (eidstate == iostate[ch]) return VALID;
  return INVALID;
}

uint32_t getSavePeriod() {
  uint32_t sp = NODECONFIG.read(EEADDR(saveperiod));
  return sp * sp * 1000UL;
}

void setMode(uint8_t ch, uint8_t mode) {
  if(ch<NUM_NATIVE_IO) pinMode(iopin[ch], mode);
  else mcp.pinMode(ch-NUM_NATIVE_IO, mode);
}
uint8_t digRead(uint8_t ch) {
  if(ch<NUM_NATIVE_IO) return digitalRead(iopin[ch]);
  else return mcp.digitalRead(ch-NUM_NATIVE_IO);
}
void digWrite(uint8_t ch, uint8_t v) {
  if(ch<NUM_NATIVE_IO) digitalWrite(iopin[ch], v);
  else mcp.digitalWrite(ch-NUM_NATIVE_IO, v);
}

// ==== Fixed: pceCallback uses stride; only ev<NUM_POS are consumers ====
void pceCallback(uint16_t index) {
  dP("\npceCallback, raw index="); dP((uint16_t)index);

  if (index < SERVO_EVENTS_TOTAL) {
    uint8_t ch = index / SERVO_EVENT_STRIDE;
    uint8_t ev = index % SERVO_EVENT_STRIDE;

    if (ev < NUM_POS) {
      curpos[ch] = ev;
      if (!servo[ch].attached()) servo[ch].attach(servopin[ch]);
      servoTarget[ch] = NODECONFIG.read(EEADDR(servos[ch].pos[ev].angle));
      dP("\n servo "); dP(ch); dP(" to pos="); dP(ev); dP(" target="); dP(servoTarget[ch]);
    }
    return;
  }

  // IO events
  index -= SERVO_EVENTS_TOTAL;
  int ch = index/2;
  uint8_t type = NODECONFIG.read(EEADDR(io[ch].type));
  if(type>=7) {
    bool inv = !(type&1);
    if( index%2 ) {
      dP("\n  OFF"); PV(ch); PV(type); PV(inv);
      digWrite(ch, inv);
      next[ch] = 0;
      iostate[ch] = 0;
    } else {
      bool pha = type<9;
      dP("\n  ON"); PV(ch); PV(pha); PV(inv);
      digWrite(ch, pha ^ inv);
      iostate[ch] = 1;
      uint8_t durn = NODECONFIG.read(EEADDR(io[ch].duration));
      if(durn) next[ch] = millis() + 100UL*durn; else next[ch]=0;
    }
  }
}

// ==== Fixed: servoProcess direction, midpoint, and reached events with stride ====
bool posdirty = false;
void servoProcess() {
  static unsigned long last = 0;
  if ((millis() - last) < servodelay) return;
  last = millis();

  static unsigned long lastmove = 0;

  for (int i = 0; i < NUM_SERVOS; i++) {
    if (servoTarget[i] == servoActual[i]) continue;

    uint8_t clpt = NODECONFIG.read(EEADDR(servos[i].pos[EVT_POS_CLOSED].angle));
    uint8_t twpt = NODECONFIG.read(EEADDR(servos[i].pos[EVT_POS_THROWN].angle));
    uint8_t midpt = (clpt + twpt) / 2;

    uint8_t oldActual = servoActual[i];
    if (servoTarget[i] > servoActual[i]) servoActual[i]++;
    else servoActual[i]--;

    if (!servo[i].attached()) {
      servo[i].attach(servopin[i]);
      delay(100);
    }

    servo[i].write(servoActual[i]);

    dP("\n servomove"); PV(i); PV(servoTarget[i]); PV(servoActual[i]);

    posdirty = true;
    lastmove = millis();

    bool goingUp = (servoActual[i] > oldActual);
    bool thrownIsHigher = (twpt >= clpt);
    uint16_t base = i * SERVO_EVENT_STRIDE;

    if (servoActual[i] == midpt) {
      bool towardThrown = (goingUp == thrownIsHigher);
      OpenLcb.produce(base + (towardThrown ? EVT_FROG_TO_THROWN : EVT_FROG_TO_CLOSED));
    }

    if (servoActual[i] == twpt) {
      OpenLcb.produce(base + EVT_REACHED_THROWN);
    }
    if (servoActual[i] == clpt) {
      OpenLcb.produce(base + EVT_REACHED_CLOSED);
    }
  }

  if (lastmove && (millis() - lastmove) > 1000) {
    for (int i = 0; i < NUM_SERVOS; i++) {
      if (servo[i].attached()) servo[i].detach();
    }
    lastmove = 0;
  }

  static unsigned long lastsave = 0;
  uint32_t saveperiod = getSavePeriod();
  if (saveperiod && posdirty && (millis() - lastsave) > saveperiod) {
    lastsave = millis();
    posdirty = false;
    for (int i = 0; i < NUM_SERVOS; i++) {
      NODECONFIG.update(EEADDR(curpos[i]), curpos[i]);
    }
    EEPROMcommit;
  }
}

void produceFromInputs() {
  const uint16_t base = IO_EVENTS_BASE;
  static uint8_t c = 0;
  static unsigned long last = 0;
  if((millis()-last)<(50/NUM_IO)) return;
  last = millis();
  uint8_t type = NODECONFIG.read(EEADDR(io[c].type));
  uint8_t d;
  bool s;
  if(type==5 || type==6) {
    s = digRead(c);
    if(s != iostate[c]) {
      iostate[c] = s;
      if(!s) {
        logstate[c] ^= 1;
        if(logstate[c]) d = NODECONFIG.read(EEADDR(io[c].duration));
        else            d = NODECONFIG.read(EEADDR(io[c].period));
        if(d==0) OpenLcb.produce( base+c*2 + logstate[c] );
        else next[c] = millis() + (uint16_t)d*100;
      }
    }
  }
  if(type>0 && type<5) {
    bool inv = !(type&1);
    s = inv ^ digRead(c);
    if(s != iostate[c]) {
      iostate[c] = s;
      d = s ? NODECONFIG.read(EEADDR(io[c].duration)) : NODECONFIG.read(EEADDR(io[c].period));
      if(d==0) OpenLcb.produce( base+c*2 + !s );
      else {
        next[c] = millis() + (uint16_t)d*100;
      }
    }
  }
  if(++c>=NUM_IO) c = 0;
}

void processProducer() {
  const uint16_t base = IO_EVENTS_BASE;
  static unsigned long last = 0;
  unsigned long now = millis();
  if( (now-last) < 50 ) return;
  for(int c=0; c<NUM_IO; c++) {
    if(next[c]==0) continue;
    if(now<next[c]) continue;
    uint8_t type = NODECONFIG.read(EEADDR(io[c].type));
    if(type>6) return;
    uint8_t s = iostate[c];
    if(type<5)  OpenLcb.produce( base+c*2 + !s );
    else OpenLcb.produce( base+c*2 + !logstate[c] );
    next[c] = 0;
  }
}

void userSoftReset() {}
void userHardReset() {}

NodeID nodeid(NODE_ADDRESS);
#include "OpenLCBMid.h"

void userConfigWritten(uint32_t address, uint16_t length, uint16_t func)
{
  dPS("\nuserConfigWritten: Addr: ", (uint32_t)address);
  dPS(" Len: ", (uint16_t)length);
  dPS(" Func: ", (uint8_t)func);
  setupIOPins();
  servodelay = NODECONFIG.read( EEADDR( servodelay ) );
  dP("\n servodelay="); dP(servodelay);
  servoSet();
}

void servoStartUp() {
  dP("\n servoStartUp");
  servodelay = NODECONFIG.read( EEADDR( servodelay ) );
  for(int i=0; i<NUM_SERVOS; i++) {
    if(getSavePeriod()==0) curpos[i] = 1;
    else curpos[i] = NODECONFIG.read( EEADDR( curpos[i] ) );
    dP("\n i="); dP(i); dP(" curpos[i]="); dP(curpos[i]);
    if( USE_90_ON_STARTUP ) servoActual[i] = 90;
    else servoActual[i] = NODECONFIG.read( EEADDR( servos[i].pos[curpos[i]].angle ) );
    dP(" servoActual[i]="); dP(servoActual[i]);
    servo[i].write(servoActual[i]);
    delay(100);
    servo[i].attach(servopin[i]);
    delay(100);
    servo[i].write(servoActual[i]);
    dP(" done");
  }
  servoSet();
}
void servoSet() {
  for(int i=0; i<NUM_SERVOS; i++) {
    servoTarget[i] = NODECONFIG.read( EEADDR( servos[i].pos[curpos[i]].angle ) );
  }
}

// ==== Fixed: setupIOPins unchanged functionally ====
void setupIOPins() {
  dP("\nPins: ");
  for(uint8_t i=0; i<NUM_IO; i++) {
    uint8_t type = NODECONFIG.read( EEADDR(io[i].type));
    if(i<NUM_NATIVE_IO) { dP("\n native "); dP(iopin[i]); dP(":"); dP(type); dP(" ="); dP(iostate[i]); }
    else { dP("\n mcp "); dP(i-NUM_NATIVE_IO); dP(":"); dP(type); dP(" ="); dP(iostate[i-NUM_NATIVE_IO]); }
    bool s;
    switch (type) {
      case 1: case 2: case 5:
        if(type==1) dP(" IN");
        if(type==2) dP(" INI");
        if(type==5) dP(" TOG");
        setMode(i, INPUT);
        s = digRead(i);
        if(type==1) iostate[i] = s;
        if(type==2) iostate[i] = !s;
        if(type==5) iostate[i] = s;
        break;
      case 3: case 4: case 6:
        if(type==3) dP(" INP");
        if(type==4) dP(" INPI");
        if(type==6) dP(" TOGP");
        setMode(i, INPUT_PULLUP);
        s = digRead(i);
        if(type==3) iostate[i] = !s;
        if(type==4) iostate[i] = s;
        if(type==6) iostate[i] = !s;
        break;
      case 7: case 8: case 9: case 10:
        if(type==7) dP(" PA");
        if(type==8) dP(" PAI");
        if(type==9) dP(" PB");
        if(type==10) dP(" PBI");
        setMode(i, OUTPUT);
        if(type==7) iostate[i] = 0;
        if(type==8) iostate[i] = 1;
        if(type==9) iostate[i] = 1;
        if(type==10) iostate[i] = 0;
        digWrite(i, iostate[i]);
        break;
    }
  }
}

void appProcess() {
  unsigned long now = millis();
  for(int i=0; i<NUM_IO; i++) {
    uint8_t type = NODECONFIG.read(EEADDR(io[i].type));
    if(type >= 7) {
      if( next[i] && now>next[i] ) {
        bool inv = !(type&1);
        bool phb = type>8;
        if(iostate[i]) {
          dP("\nphaseB"); PV(i); PV(phb); PV(inv); PV(phb ^ inv);
          digWrite(i, phb ^ inv);
          iostate[i] = 0;
          if( NODECONFIG.read(EEADDR(io[i].period)) > 0 )
            next[i] = now + 100UL*NODECONFIG.read(EEADDR(io[i].period));
          else next[i] = 0;
        } else {
          dP("\nphaseA"); PV(i); PV(phb); PV(inv); PV(!phb ^ inv);
          digWrite(i, !phb ^ inv);
          iostate[i] = 1;
          if( NODECONFIG.read(EEADDR(io[i].duration)) > 0 )
            next[i] = now + 100UL*NODECONFIG.read(EEADDR(io[i].duration));
          else next[i] = 0;
        }
      }
    }
  }
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200); while(!Serial);
  delay(500);
  dP("\n AVR-2Servo24IOCAN");
  dP("\n num servos = "); dP(NUM_SERVOS);
  dP("\n num servo pos = "); dP(NUM_POS);
  dP("\n num native io = "); dP(NUM_NATIVE_IO);
  dP("\n num mcp groups = "); dP(NUM_MCP_GROUPS);
  dP("\n num io in each mcp group"); dP(NUM_MCP_IO_PER_GROUP);
  dP("\n num mcp io = "); dP(NUM_MCP_IO);
  dP("\n total num io = "); dP(NUM_IO);
  dP("\n num events = "); dP(NUM_EVENT);
#endif

  WIRE_begin;
  EEPROMbegin;

  Olcb_init(nodeid, RESET_TO_FACTORY_DEFAULTS);
  setupIOPins();
  servoStartUp();
  dP("\n NUM_EVENT="); dP(NUM_EVENT);

  // Removed test writes that forced both ends to 90 on servo 0
}

void loop() {
  bool activity = Olcb_process();
#ifndef OLCB_NO_BLUE_GOLD
  if (activity) { blue.blink(0x1); }
  if (olcbcanTx.active) {
    gold.blink(0x1);
    olcbcanTx.active = false;
  }
  gold.process();
  blue.process();
#endif
  produceFromInputs();
  appProcess();
  servoProcess();
  processProducer();
}
