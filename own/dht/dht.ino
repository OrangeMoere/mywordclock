#include "DHT.h"
 
#define DHT_TYPE DHT22
#define PIN_DHT D5

const int DHT_PIN = 5;
DHT dht(PIN_DHT, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

// runs over and over again
void loop() {
  uint16_t h = 10*dht.readHumidity();
  uint16_t t = 10*dht.readTemperature();
  
  Serial.print("Temperatur: ");
  Serial.print(t);
  Serial.print("°C, Luftfeuchtigkeit: ");
  Serial.print(h);
  Serial.println("%");
 
  delay(3000);
}
