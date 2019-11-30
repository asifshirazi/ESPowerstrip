#include <FS.h>                   // this needs to be first, or it all crashes and burns...
#define BLYNK_PRINT Serial        // comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>

#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>

#include "settings.h"
#include "wifimanagment.h"
#include "shiftwrite.h"
#include "temperature.h"

BlynkTimer timer;

void checkPhysicalButton();

int relayState[] = { 1, 1, 1, 1, 0, 1 };
int btnState[] = { 1, 1, 1, 1, 0, 1 };

// Every time we connect to the cloud...
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(0,1,2,3,4,5,6);

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
  for (int i = 0; i < TOTAL_RELAYS; i++) {
    if (digitalRead(BTN_PIN[i]) == 0) {
      // btnState[] is used to avoid sequential toggles
      if (btnState[i] != 0) {
        // Toggle RELAY state
        relayState[i] = !relayState[i];
        shiftWrite(i, relayState[i]);
        // Update Button Widget
        Blynk.virtualWrite(i, relayState[i]);
        digitalWrite(i, btnState[i]);
        Serial.println(BTN_DEVICE[i] + String(" button pressed..."));
      }
      btnState[i] = 0;
      digitalWrite(BTN_PIN[i], btnState[i]);
    } else {
      btnState[i] = 1;
      digitalWrite(BTN_PIN[i], btnState[i]);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Set the SPI pins to be outputs
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(THERMO_PIN, OUTPUT);

  for (int i = 0; i < TOTAL_RELAYS; i++) {
    // Turn off all RELAYS
    shiftWrite(i, HIGH);
    // Restore previous RELAYS states
    pinMode(BTN_PIN[i], INPUT_PULLUP);
    shiftWrite(i, relayState[i]);
  }

  //set wifi blink led pin as output
  pinMode(BLED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //SPIFFS.format();    //clean FS, for testing
  Serial.println("Mounting FS...");    //read configuration from FS json

  if (SPIFFS.begin()) {
    Serial.println("Mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("Reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("Opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(blynk_token, json["blynk_token"]);

        } else {
          Serial.println("Failed to load json config");
        }
      }
    }
  } else {
    Serial.println("Failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 33);   // was 32 length
  
  Serial.println(blynk_token);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  wifiManager.setSaveConfigCallback(saveConfigCallback);   //set config save notify callback

  //set static ip
  // this is for connecting to Office router not GargoyleTest but it can be changed in AP mode at 192.168.4.1
  //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,10,111), IPAddress(192,168,10,90), IPAddress(255,255,255,0));
  
  wifiManager.addParameter(&custom_blynk_token);   //add all your parameters here

  //wifiManager.resetSettings();  //reset settings - for testing

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep, in seconds
  wifiManager.setTimeout(600);   // 10 minutes to enter data and then Wemos resets to try again.

  //fetches ssid and pass and tries to connect, if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("ESPowersrtip", "Welcome5")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
  Serial.println("Connected to ESPowerstrip :)");   //if you get here you have connected to the WiFi
  ticker.detach();
  //turn LED off
  digitalWrite(BLED, HIGH);

  strcpy(blynk_token, custom_blynk_token.getValue());    //read updated parameters

  if (shouldSaveConfig) {      //save the custom parameters to FS
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("Failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  
  Blynk.config(blynk_token);
  Blynk.connect();
  sensors.begin();

  // Setup function to be called every time
  timer.setInterval(100L, checkPhysicalButton);
  timer.setInterval(1000L, sendTemps);
}

void loop() {
  Blynk.run();
  timer.run();
}
