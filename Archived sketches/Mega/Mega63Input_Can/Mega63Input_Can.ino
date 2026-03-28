//==============================================================
// AVR 63Inputs using ACAN for Arduino Mega
//
// Copyright 2024 David P Harris
// Derived from work by Alex Shepherd and David Harris
// Updated 2024.11.14, modified 2025.09.01 for 63 inputs on Arduino Mega
//==============================================================
// - 63 input channels (pins 3-49, A0-A15):
//     - type: 0=None,
//             1=Input, 2=Input Inverted,
//             3=Input with pull-up, 4=Input with pull-up Inverted,
//             5=Input toggle, 6=Toggle with pull-up.
//     - Events are produced
//     - On-delay: delay before on-event is sent
//     - Off-delay: the period before the off-event is sent
// - Reserved pins: 0, 1 (serial), 2 (interrupt), 50 (MISO), 51 (MOSI), 52 (SCK), 53 (CS for MCP2515)
//==============================================================

// Debugging -- uncomment to activate debugging statements:
//#define DEBUG Serial

// Allow direct to JMRI via USB, without CAN controller, comment out for CAN
// (Note: disable debugging if this is chosen.)
//#include "GCSerial.h"

#include <mdebugging.h>

#include "user_config.h"

// New ACan for MCP2515
#define ACAN_FREQ 8000000UL   // set for crystal freq feeding the MCP2515 chip
#define ACAN_CS_PIN 53        // set for the MCP2515 chip select pin (Mega SPI CS)
#define ACAN_INT_PIN 2        // set for the MCP2515 interrupt pin
#define ACAN_RX_NBUF 2        // number of receive buffers
#define ACAN_TX_NBUF 2        // number of transmit buffers
#if defined(ARDUINO_AVR_MEGA2560)
  #include "ACan.h"     // uses main library ACan class
#endif

// Board definitions
#define MANU "J Holmes"        // The manufacturer of node
#define MODEL "AVR63Inputs"    // The model of the board
#define HWVERSION "0.1"        // Hardware version
#define SWVERSION "0.4"        // Software version, updated for 63-input Mega version

// User defs
#define NUM_IO 63
#define NUM_EVENT NUM_IO * 2

#include "processCAN.h"           // Auto-select CAN library
#include "processor.h"            // auto-selects the processor type, EEPROM lib etc.
#include "OpenLCBHeader.h"        // System house-keeping.

// CDI (Configuration Description Information) in XML, must match MemStruct
extern "C" {
    #define N(x) xN(x)     // allow the insertion of the value (x) ..
    #define xN(x) #x       // .. into the CDI string.
const char configDefInfo[] PROGMEM =
// ===== Enter User definitions below =====
  CDIheader R"(
    <name>Application Configuration</name>
    <group replication=')" N(NUM_IO) R"('>
        <name>Inputs</name>
        <repname>Input </repname>
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
            <name>On-delay 1-255 = 100ms-25.5s, 0=steady-state</name>
            <hints><slider tickSpacing='50' immediate='yes' showValue='yes'> </slider></hints>
        </int>
        <int size='1'>
            <name>Off-delay 1-255 = 100ms-25.5s, 0=No repeat</name>
            <hints><slider tickSpacing='50' immediate='yes' showValue='yes'> </slider></hints>
        </int>
        <eventid><name>On-Event</name></eventid>
        <eventid><name>Off-Event</name></eventid>
    </group>
    )" CDIfooter;
// ===== Enter User definitions above =====
} // end extern

// ===== MemStruct =====
// Memory structure of EEPROM, must match CDI above
typedef struct {
    EVENT_SPACE_HEADER eventSpaceHeader; // MUST BE AT THE TOP OF STRUCT - DO NOT REMOVE!!!
    char nodeName[20];  // optional node-name, used by ACDI
    char nodeDesc[24];  // optional node-description, used by ACDI
    // ===== Enter User definitions below =====
    struct {
        char desc[8];
        uint8_t type;
        uint8_t duration;    // 100ms-25.5s, 0=steady for On-delay
        uint8_t period;      // 100ms-25.5s, 0=no repeat for Off-delay
        EventID onEid;
        EventID offEid;
    } io[NUM_IO];
    // ===== Enter User definitions above =====
} MemStruct;                 // type definition

// Pin assignments for Arduino Mega (pins 3-49, A0-A15)
uint8_t iopin[NUM_IO] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, // Digital pins 3-49
    A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15 // Analog pins A0-A15
};

uint8_t iostate[NUM_IO];
bool logstate[NUM_IO] = {0}; // logic state for toggle
unsigned long next[NUM_IO] = {0};

extern "C" {
    // ===== eventid Table =====
    #define REG_IO(i) PCEID(io[i].onEid), PCEID(io[i].offEid)
    
    // Array of the offsets to every eventID in MemStruct/EEPROM/mem, and P/C flags
    const EIDTab eidtab[NUM_EVENT] PROGMEM = {
        REG_IO(0), REG_IO(1), REG_IO(2), REG_IO(3), REG_IO(4), REG_IO(5),
        REG_IO(6), REG_IO(7), REG_IO(8), REG_IO(9), REG_IO(10), REG_IO(11),
        REG_IO(12), REG_IO(13), REG_IO(14), REG_IO(15), REG_IO(16), REG_IO(17),
        REG_IO(18), REG_IO(19), REG_IO(20), REG_IO(21), REG_IO(22), REG_IO(23),
        REG_IO(24), REG_IO(25), REG_IO(26), REG_IO(27), REG_IO(28), REG_IO(29),
        REG_IO(30), REG_IO(31), REG_IO(32), REG_IO(33), REG_IO(34), REG_IO(35),
        REG_IO(36), REG_IO(37), REG_IO(38), REG_IO(39), REG_IO(40), REG_IO(41),
        REG_IO(42), REG_IO(43), REG_IO(44), REG_IO(45), REG_IO(46), REG_IO(47),
        REG_IO(48), REG_IO(49), REG_IO(50), REG_IO(51), REG_IO(52), REG_IO(53),
        REG_IO(54), REG_IO(55), REG_IO(56), REG_IO(57), REG_IO(58), REG_IO(59),
        REG_IO(60), REG_IO(61), REG_IO(62)
    };
    
    // SNIP Short node description for use by the Simple Node Information Protocol
    extern const char SNII_const_data[] PROGMEM = "\001" MANU "\000" MODEL "\000" HWVERSION "\000" OlcbCommonVersion ; // last zero in double-quote
} // end extern "C"

// PIP Protocol Identification Protocol uses a bit-field to indicate which protocols this node supports
uint8_t protocolIdentValue[6] = {
    pSimple | pDatagram | pMemConfig | pPCEvents | !pIdent | pTeach | !pStream | !pReservation, // 1st byte
    pACDI   | pSNIP     | pCDI       | !pRemote | !pDisplay | !pTraction | !pFunction | !pDCC, // 2nd byte
    0, 0, 0, 0                                                                                           // remaining 4 bytes
};

#define OLCB_NO_BLUE_GOLD  // blue/gold not used in this sketch

// Initialize EEPROM to Factory Defaults
void userInitAll() {
    NODECONFIG.put(EEADDR(nodeName), ESTRING("AVR Mega"));
    NODECONFIG.put(EEADDR(nodeDesc), ESTRING("63Inputs"));
    for (uint8_t i = 0; i < NUM_IO; i++) {
        NODECONFIG.put(EEADDR(io[i].desc), ESTRING(""));
        NODECONFIG.update(EEADDR(io[i].type), 0);
        NODECONFIG.update(EEADDR(io[i].duration), 0);
        NODECONFIG.update(EEADDR(io[i].period), 0);
        iostate[i] = 0xFF;
    }
}

// Determine the state of each eventid
enum evStates { VALID=4, INVALID=5, UNKNOWN=7 };
uint8_t userState(uint16_t index) {
    uint8_t ch = index / 2;
    uint8_t type = NODECONFIG.read(EEADDR(io[ch].type));
    if (type == 0) return UNKNOWN;
    bool s = digitalRead(iopin[ch]);
    if (!s ^ (type & 1)) return VALID;
    else return INVALID;
}

// Process Consumer-eventIDs (no action for inputs-only)
void pceCallback(uint16_t index) {
    dP("\npceCallback, index="); dP((uint16_t)index);
    // Inputs do not consume events, so no action needed
}

// Process Inputs
void produceFromInputs() {
    const uint8_t base = 0;
    static uint8_t c = 0;
    static unsigned long last = 0;
    if ((millis() - last) < (50 / NUM_IO)) return;
    last = millis();
    uint8_t type = NODECONFIG.read(EEADDR(io[c].type));
    uint8_t d;
    if (type == 5 || type == 6) {
        bool s = digitalRead(iopin[c]);
        if (s != iostate[c]) {
            iostate[c] = s;
            if (!s) {
                logstate[c] ^= 1;
                if (logstate[c]) d = NODECONFIG.read(EEADDR(io[c].duration));
                else d = NODECONFIG.read(EEADDR(io[c].period));
                if (d == 0) OpenLcb.produce(base + c * 2 + logstate[c]); // no delay, send event
                else next[c] = millis() + (uint16_t)d * 100; // register delay
            }
        }
    }
    if (type > 0 && type < 5) {
        bool s = digitalRead(iopin[c]);
        if (s != iostate[c]) {
            iostate[c] = s;
            if (!iostate[c]) d = NODECONFIG.read(EEADDR(io[c].duration));
            else d = NODECONFIG.read(EEADDR(io[c].period));
            if (d == 0) OpenLcb.produce(base + c * 2 + (!s ^ (type & 1))); // no delay, send event
            else next[c] = millis() + (uint16_t)d * 100; // register delay
        }
    }
    if (++c >= NUM_IO) c = 0;
}

// Process pending producer events
void processProducer() {
    const uint8_t base = 0;
    static unsigned long last = 0;
    unsigned long now = millis();
    if ((now - last) < 50) return;
    for (int c = 0; c < NUM_IO; c++) {
        if (next[c] == 0) continue;
        if (now < next[c]) continue;
        uint8_t type = NODECONFIG.read(EEADDR(io[c].type));
        if (type > 6) continue; // no outputs to process
        uint8_t s = iostate[c];
        if (type < 5) OpenLcb.produce(base + c * 2 + (!s ^ (type & 1))); // regular inputs
        else OpenLcb.produce(base + c * 2 + logstate[c]); // toggle inputs
        next[c] = 0;
    }
}

// Setup the input pins
void setupIOPins() {
    dP("\nPins: ");
    for (uint8_t i = 0; i < NUM_IO; i++) {
        uint8_t type = NODECONFIG.read(EEADDR(io[i].type));
        switch (type) {
            case 1: case 2: case 5:
                dP(" IN:");
                pinMode(iopin[i], INPUT);
                iostate[i] = 0xFF; // trigger first send
                break;
            case 3: case 4: case 6:
                dP(" INP:");
                pinMode(iopin[i], INPUT_PULLUP);
                iostate[i] = 0xFF;
                break;
            default:
                dP(" NONE:");
                pinMode(iopin[i], INPUT); // default to input for safety
                iostate[i] = 0xFF;
                break;
        }
        dP(iopin[i]); dP(":"); dP(type); dP(", ");
    }
}

// Callback from a Configuration write
void userConfigWritten(uint32_t address, uint16_t length, uint16_t func) {
    dPS("\nuserConfigWritten: Addr: ", (uint32_t)address);
    dPS(" Len: ", (uint16_t)length);
    dPS(" Func: ", (uint8_t)func);
    setupIOPins();
}

void userSoftReset() {}
void userHardReset() {}

#include "OpenLCBMid.h"    // Essential, do not move or delete

// Setup does initial configuration
void setup() {
    #ifdef DEBUG
        Serial.begin(115200); while (!Serial);
        delay(500);
        dP("\n AVR-63Inputs");
    #endif

    NodeID nodeid(NODE_ADDRESS);       // this node's nodeid
    Olcb_init(nodeid, RESET_TO_FACTORY_DEFAULTS);
    setupIOPins();
    dP("\n NUM_EVENT="); dP(NUM_EVENT);
}

// Loop
void loop() {
    bool activity = Olcb_process();
    produceFromInputs();  // scans inputs and generates events on change
    processProducer();    // processes delayed producer events from inputs
}