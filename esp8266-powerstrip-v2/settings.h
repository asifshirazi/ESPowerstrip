/* You should get Auth Token in the Blynk app */
char auth[] = "bIuXGGq6sw7dsy9n4Iu-Tlf0HhJEUIIO";

#define TOTAL_RELAYS 6

/**************************************************
               SN74HC595 Connections
**************************************************/

/* Controls the internal transference of data in SN74HC595 internal registers */
const int LATCH_PIN = 16; // D0 - Connected to ST_CP
/* Generates the clock signal to control the transference of data */
const int CLOCK_PIN = 5;  // D1 - Connected to SH_CP
/* Outputs the byte to transfer */
const int DATA_PIN = 15;   // D8 - Connected to DS


/**************************************************
   NodeMCU to Thermosensor & Push Button switch
**************************************************/
 
// D3, D4, D5, D6, D7, RX
// V0, V1, V2, V3, V4, V5
// LG, CO, HE, CH, WF, RG
int BTN_PIN[] = {0, 2, 14, 12, 13, 3};

const int THERMO_PIN = 4; // D2
