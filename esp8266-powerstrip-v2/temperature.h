OneWire oneWire(THERMO_PIN);
DallasTemperature sensors(&oneWire);

float  temp = 0;

void sendTemps(){
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.println(String("Water = ")+temp+ String(" °C"));
  Blynk.virtualWrite(V6, temp);
}
