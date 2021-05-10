

#define WIFISSID "nuresp"
#define WIFIPW  "esp8266!"

#define DEBUGLEVEL 2

#define LED_NUMBER 110
#define WORD_NUMBER 22

#define HUE_STEP 100
#define VALUE_STEP 1
#define VALUE_MULTIPLIER 0.5
#define VALUE_OFFSET 128

/* Configuration of Pins */
#define PIN_LED D6
#define PIN_DHT D5

/* Configuration of NTP */
#define NTP_SERVER1 "192.168.178.1"
#define NTP_SERVER2 "de.pool.ntp.org"
#define TZ "CET-1CEST,M3.5.0/2:00,M10.5.0/3:00"

/* Update Intervals */
#define INTERVAL_UPDATETIME 900
#define INTERVAL_PRINTTIME 1000
#define INTERVAL_UPDATENTP 1000*60*10
#define INTERVAL_UPDATEWORDS 950
#define INTERVAL_UPDATEPIXELS 10
#define INTERVAL_UPDATEVALUEFROMANALOGINPUT 1000
#define INTERVAL_CIRCLEHUE 100

/* Necessary Includes */
#include <ESP8266WiFi.h>
#include <time.h>
#include <coredecls.h>
#include <Adafruit_NeoPixel.h>

/* Globals */

/* previousMillis (pm) */
unsigned long pm_updateTime = 0;
unsigned long pm_printTime = 0;
unsigned long pm_updateWords = 0;
unsigned long pm_updatePixels = 0;
unsigned long pm_updateValueFromAnalogInput = 0;
unsigned long pm_circleHue = 0;

tm tm;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_NUMBER, PIN_LED, NEO_GRB + NEO_KHZ800);

uint8_t curWordsValue[WORD_NUMBER];
uint8_t tarWordsStatus[WORD_NUMBER];
uint8_t tarWordsValue = 100;

uint8_t tarWordsSaturation = 255; //set 0 for white
uint16_t tarWordsHue = 32768;
/* Rot: 0
 * Gelb: 10922
 * Grün: 21845
 * Türkis: 32768
 * Blau: 43690
 * Violett: 54613
 */

const uint8_t addresses[WORD_NUMBER][2] = {
  {60, 4},  //Eins     0
  {62, 4},  //Zwei     1
  {67, 4},  //Drei     2
  {77, 4},  //Vier     3
  {73, 4},  //Fünf     4
  {104, 5}, //Sechs    5
  {55, 6},  //Sieben   6
  {89, 4},  //Acht     7
  {81, 4},  //Neun     8
  {93, 4},  //Zehn     9
  {85, 3},  //Elf     10
  {49, 5},  //Zwölf   11
  {0, 2},   //Es      12
  {3, 3},   //Ist     13
  {7, 4},   //Fünf    14
  {18, 4},  //Zehn    15
  {26, 7},  //Viertel 16
  {11, 7},  //Zwanzig 17
  {44, 4},  //Halb    18
  {38, 4},  //Nach    19
  {35, 3},  //Vor     20
  {99, 3}   //Uhr     21
};

#define ES 12
#define IST 13
#define FUENF 14
#define ZEHN 15
#define VIERTEL 16
#define ZWANZIG 17
#define HALB 18
#define NACH 19
#define VOR 20
#define UHR 21

void setup() {
  if (DEBUGLEVEL > 0) Serial.begin(115200);

  pixels.begin();
  pixels.clear();
  pixels.show();
  
  // start network
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFISSID, WIFIPW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (DEBUGLEVEL > 0) Serial.print(".");
  }
  if (DEBUGLEVEL > 0) Serial.println("\nWiFi connected");

  configTime(TZ, NTP_SERVER1, NTP_SERVER2);
  if (DEBUGLEVEL > 0) settimeofday_cb(time_is_set);
  
  resetWords(curWordsValue);
  resetWords(tarWordsStatus);
  //sntp_real_timestamp = 0;
}

void loop() {
  updateTime();
  printTime();
  updateWords();
  updatePixels();
  updateValueFromAnalogInput();
  //circleHue();
}

void circleHue() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_circleHue <= INTERVAL_CIRCLEHUE) {
    return;
  }
  pm_circleHue = currentMillis;

  tarWordsHue += HUE_STEP;
}

void updateValueFromAnalogInput() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateValueFromAnalogInput <= INTERVAL_UPDATEVALUEFROMANALOGINPUT) {
    return;
  }
  pm_updateValueFromAnalogInput = currentMillis;
  
  int32_t a0 = analogRead(A0); //liefert Werte von 1-1024
  a0 -= 1;
  a0 = VALUE_MULTIPLIER * a0 + VALUE_OFFSET;
  a0 /= 4;
  if (a0 < 0) a0 = 0;
  if (a0 > 255) a0 = 255;
  tarWordsValue = a0; //akzeptiert Werte von 0-255
}

void updatePixels() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updatePixels <= INTERVAL_UPDATEPIXELS) {
    return;
  }
  pm_updatePixels = currentMillis;
  
  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    if ((tarWordsStatus[i]*tarWordsValue != curWordsValue[i])) {
      int8_t increase = 1;
      if (tarWordsStatus[i]*tarWordsValue < curWordsValue[i]) increase = -1;
      int32_t value = curWordsValue[i] + increase*VALUE_STEP;
      if (value < 0) value = 0;
      if (value > 255) value = 255;
      curWordsValue[i] = value;
      uint32_t rgbcolor = pixels.gamma32(pixels.ColorHSV(tarWordsHue, tarWordsSaturation, value));
      pixels.fill(rgbcolor, addresses[i][0], addresses[i][1]);
      if (DEBUGLEVEL > 2) Serial.println(value);
    }
  }
  pixels.show();
}

void resetWords(uint8_t *words) {
  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    words[i] = 0;
  }
}

void time_is_set() { //Diese Funktion wird als Rückruf festgelegt, wenn Zeitdaten abgerufen wurden.
  if (DEBUGLEVEL > 0) Serial.println("********* NTP Server Timestap Synchronisation  *********");
  //sntp_real_timestamp = 0;
}

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 () { // SNTP-Aktualisierungsverzögerung ändern, ohne diese Funktion wird jede Stunde die Zeit geholt.
  return INTERVAL_UPDATENTP;
}

void updateTime() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateTime <= INTERVAL_UPDATETIME) {
    return;
  }
  pm_updateTime = currentMillis;

  time_t now = time(nullptr); // read the current time
  localtime_r(&now, &tm); // update the structure tm with the current time
}

void printTime() {
  if (DEBUGLEVEL <= 1) {
    return;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - pm_printTime <= INTERVAL_PRINTTIME) {
    return;
  }
  pm_printTime = currentMillis;

  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);  // years since 1900
  Serial.print("  month:");
  Serial.print(tm.tm_mon + 1);      // January = 0 (!)
  Serial.print("  day:");
  Serial.print(tm.tm_mday);         // day of month
  Serial.print("  hour:");
  Serial.print(tm.tm_hour);         // hours since midnight  0-23
  Serial.print("  min:");
  Serial.print(tm.tm_min);          // minutes after the hour  0-59
  Serial.print("  sec:");
  Serial.print(tm.tm_sec);          // seconds after the minute  0-61*
  Serial.print("  wday");
  Serial.print(tm.tm_wday);         // days since Sunday 0-6
  if (tm.tm_isdst == 1)             // Daylight Saving Time flag
    Serial.print("  DST");
  else
    Serial.print("  standard");
  Serial.println();
}

void updateWords() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateWords <= INTERVAL_UPDATEWORDS) {
    return;
  }
  pm_updateWords = currentMillis;

  resetWords(tarWordsStatus);
  tarWordsStatus[ES] = 1;
  tarWordsStatus[IST] = 1;

  // STUNDENZAHL
  uint8_t index_hour = tm.tm_hour;
  if (tm.tm_min > 22 || tm.tm_min == 22 && tm.tm_sec >= 30) {
    index_hour++;
  }
  if (index_hour == 0) index_hour = 12;
  if (index_hour > 12) index_hour -= 12;
  index_hour--;
  tarWordsStatus[index_hour] = 1;

  if ((tm.tm_min > 57 || tm.tm_min == 57 && tm.tm_sec >= 30) || (tm.tm_min < 2 || tm.tm_min == 2 && tm.tm_sec < 30)) {
    tarWordsStatus[UHR] = 1;
  }

  if ((tm.tm_min > 2 || tm.tm_min == 2 && tm.tm_sec >= 30) && (tm.tm_min < 7 || tm.tm_min == 7 && tm.tm_sec < 30)) {
    tarWordsStatus[FUENF] = 1;
    tarWordsStatus[NACH] = 1;
  }

  if ((tm.tm_min > 7 || tm.tm_min == 7 && tm.tm_sec >= 30) && (tm.tm_min < 12 || tm.tm_min == 12 && tm.tm_sec < 30)) {
    tarWordsStatus[ZEHN] = 1;
    tarWordsStatus[NACH] = 1;
  }

  if ((tm.tm_min > 12 || tm.tm_min == 12 && tm.tm_sec >= 30) && (tm.tm_min < 17 || tm.tm_min == 17 && tm.tm_sec < 30)) {
    tarWordsStatus[VIERTEL] = 1;
    tarWordsStatus[NACH] = 1;
  }

  if ((tm.tm_min > 17 || tm.tm_min == 17 && tm.tm_sec >= 30) && (tm.tm_min < 22 || tm.tm_min == 22 && tm.tm_sec < 30)) {
    tarWordsStatus[ZWANZIG] = 1;
    tarWordsStatus[NACH] = 1;
  }

  if ((tm.tm_min > 22 || tm.tm_min == 22 && tm.tm_sec >= 30) && (tm.tm_min < 27 || tm.tm_min == 27 && tm.tm_sec < 30)) {
    tarWordsStatus[FUENF] = 1;
    tarWordsStatus[VOR] = 1;
    tarWordsStatus[HALB] = 1;
  }

  if ((tm.tm_min > 27 || tm.tm_min == 27 && tm.tm_sec >= 30) && (tm.tm_min < 32 || tm.tm_min == 32 && tm.tm_sec < 30)) {
    tarWordsStatus[HALB] = 1;
  }

  if ((tm.tm_min > 32 || tm.tm_min == 32 && tm.tm_sec >= 30) && (tm.tm_min < 37 || tm.tm_min == 37 && tm.tm_sec < 30)) {
    tarWordsStatus[FUENF] = 1;
    tarWordsStatus[NACH] = 1;
    tarWordsStatus[HALB] = 1;
  }

  if ((tm.tm_min > 37 || tm.tm_min == 37 && tm.tm_sec >= 30) && (tm.tm_min < 42 || tm.tm_min == 42 && tm.tm_sec < 30)) {
    tarWordsStatus[ZWANZIG] = 1;
    tarWordsStatus[VOR] = 1;
  }

  if ((tm.tm_min > 42 || tm.tm_min == 42 && tm.tm_sec >= 30) && (tm.tm_min < 47 || tm.tm_min == 47 && tm.tm_sec < 30)) {
    tarWordsStatus[VIERTEL] = 1;
    tarWordsStatus[VOR] = 1;
  }

  if ((tm.tm_min > 47 || tm.tm_min == 47 && tm.tm_sec >= 30) && (tm.tm_min < 52 || tm.tm_min == 52 && tm.tm_sec < 30)) {
    tarWordsStatus[ZEHN] = 1;
    tarWordsStatus[VOR] = 1;
  }

  if ((tm.tm_min > 52 || tm.tm_min == 52 && tm.tm_sec >= 30) && (tm.tm_min < 57 || tm.tm_min == 57 && tm.tm_sec < 30)) {
    tarWordsStatus[FUENF] = 1;
    tarWordsStatus[VOR] = 1;
  }
}
