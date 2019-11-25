/* You should get Auth Token in the Blynk app */
char auth[] = "bIuXGGq6sw7dsy9n4Iu-Tlf0HhJEeJiP";

/**************************************************
               SN74HC595 Connections
**************************************************/

/* Controls the internal transference of data in SN74HC595 internal registers */
const int LATCH_PIN = 16; // D0 - Connected to ST_CP
/* Generates the clock signal to control the transference of data */
const int CLOCK_PIN = 5;  // D1 - Connected to SH_CP
/* Outputs the byte to transfer */
const int DATA_PIN = 4;   // D2 - Connected to DS


/**************************************************
          NodeMCU to Push Button switch
**************************************************/
 
// D3, D4, D5, D6, D7, RX
int BTN_PIN[] = {0, 2, 14, 12, 13, 3};
