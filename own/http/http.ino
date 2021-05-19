#define ARDUINOJSON_USE_DOUBLE 1

#define LED_NUMBER 110
#define LED_PIN D6
#define JSON_LENGTH 5000

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "nuresp";
const char* password = "esp8266!";

//Your Domain name with URL path or IP address with path
const char* serverName = "https://luetzenkirchens.de/led/led1.json";

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_NUMBER, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.clear();
  pixels.show();
  
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
}
void loop() {
  getInfos();
}

void getInfos() {
  String download = httpGETRequest(serverName);
  Serial.println(download);
  StaticJsonDocument<24576> json;
  DeserializationError error = deserializeJson(json, download);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  } else {
    Serial.println(download);
    /*uint8_t led [110];
    led = json["led"];
    char color[110][7];
    color = json["color"];

    for (uint8_t i = 0; i < LED_NUMBER; i++) {
      uint8_t r = strtol(color+1, color+3);
      uint8_t g = strtol(color+2, color+4);
      uint8_t b = strtol(color+3, color+5);
      pixels.setPixelColor(i,r,g,b);
    }*/
    
    
  }
  
  delay(10000);
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
