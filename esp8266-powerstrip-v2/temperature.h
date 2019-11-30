OneWire oneWire(THERMO_PIN);
DallasTemperature sensors(&oneWire);

float  temp = 0;

void sendTemps(){
  sensors.setWaitForConversion(true);
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.println(String("Water = ")+temp+ String(" °C"));
  if (temp == 85.0 || temp == (-127.0)){
    Serial.println("Syncing temperature");
  } else {
    Blynk.virtualWrite(V6, temp);
  }
}
