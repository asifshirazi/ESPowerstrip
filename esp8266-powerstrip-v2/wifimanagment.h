Ticker ticker;

bool shouldSaveConfig = false; // flag for saving data
int BLED = 2;

void tick() {
  int state = digitalRead(BLED);  // get the current state of LED
  digitalWrite(BLED, !state);     // set pin to the opposite state
}

void saveConfigCallback () {  //callback notifying us of the need to save config
  Serial.println("Should save config");
  shouldSaveConfig = true;
  ticker.attach(0.2, tick);  // led toggle faster
}
