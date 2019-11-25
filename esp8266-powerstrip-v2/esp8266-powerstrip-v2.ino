/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include "settings.h"
#include "wifimanagment.h"
#include "shiftwrite.h"
#include <BlynkSimpleEsp8266.h>

BlynkTimer timer;

void checkPhysicalButton();

int relayState[] = { 1, 1, 1, 1, 1, 1 };
int btnState[] = { 1, 1, 1, 1, 1, 1 };

// Every time we connect to the cloud...
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(0,1,2,3,4,5);

  // Alternatively, you could override server state using:
  //Blynk.virtualWrite(V2, relayState1);
}

// When App button is pushed - switch the state
BLYNK_WRITE_DEFAULT() {
  int pin = request.pin; // determines what vPin is triggering this response
  relayState[pin] = param.asInt();
  shiftWrite(pin, relayState[pin]);
}

void checkPhysicalButton() {
  for (int i = 0; i <= 5; i++) {
    if (digitalRead(BTN_PIN[i]) == 0) {
      // btnState[] is used to avoid sequential toggles
      if (btnState[i] != 0) {
        // Toggle RELAY state
        relayState[i] = !relayState[i];
        shiftWrite(i, relayState[i]);
        // Update Button Widget
        Blynk.virtualWrite(i, relayState[i]);
      }
      btnState[i] = 0;
    } else {
      btnState[i] = 1;
    }
  }
}

void setup() {
  EEPROM.begin(memalloc);
  Serial.begin(115200);
  delay(100);

  // Set the three SPI pins to be outputs
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  for (int i = 0; i <= 5; i++) {
    // Turn off all RELAYS
    shiftWrite(i, HIGH);
    // Restore previous RELAYS states
    pinMode(BTN_PIN[i], INPUT_PULLUP);
    shiftWrite(i, relayState[i]);
  }

  // Setup a function to be called every 100 ms
  timer.setInterval(100L, checkPhysicalButton);

  if(connectWifi()){
    Blynk.begin(auth, ssid, password);
  } else {
    startAP();
    Blynk.begin(auth, ssid, password);
  }
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    Blynk.run();
    timer.run();
  } else{
    startAP();
  }
}
