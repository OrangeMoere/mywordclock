
#define WIFISSID ""
#define WIFIPW ""

#define DEBUGLEVEL 1 //set 0 for no Serial Messages

#define USE_ANALOG_INPUT 0 //1=on, 0=off
#define COLORMODE 0
/*
   Color is described by Hue(16 Bit) and Saturation(8 Bit).
   Use tarValue below for fix Value or activate USE_ANALOG_INPUT;
   Use INTERVAL_CIRCLECOLOR for interval the color change (ms)

   Colormode 0: all Words have the same Color all the Time, hue/saturation index from circleColorIndex below
   Colormode 1: all Words have the same Color, but Color is changing with time
   Colormode 2: all Words have the same Color, but Color is changing with day of week,
                hue/saturation index 1-7, starting with sunday
   Colormode 3: all Words have different Colors, without changing by time
   Colormode 4: all Words have different Colors, changing by time
*/

#define LED_NUMBER 110
#define WORD_NUMBER 23
#define COLOR_NUMBER 9 //>=9

#define HUE_STEP 200
#define SATURATION_STEP 1
#define VALUE_STEP 1

#define VALUE_MULTIPLIER 1
#define VALUE_OFFSET 100

/* Configuration of Pins */
#define PIN_LED D6

/* Configuration of NTP */
#define NTP_SERVER1 "de.pool.ntp.org" //Allgemeiner NTP Server
#define UTC_OFFSET 7200 //in Sekunden
//#define NTP_SERVER2 "de.pool.ntp.org" //fritz box NTP Server 192.168.178.1
//#define TZ "CET-1CEST,M3.5.0/2:00,M10.5.0/3:00" //Sommer Winter Zeit
#define DEBUGTIMESTAMP 1621679000
#define DEBUGTIMESTAMPACTIVATE 0

/* Update Intervals */
#define INTERVAL_UPDATENTP 1000*60*10 //Millisekunden wie oft NTP Anfrage gestellt werden soll

#define INTERVAL_UPDATEVALUEFROMANALOGINPUT 1000
#define INTERVAL_CIRCLECOLOR 1000*10

#define INTERVAL_UPDATETIME 1000
#define INTERVAL_PRINTTIME 1000
#define INTERVAL_UPDATEWORDSTATUS 1000

#define INTERVAL_UPDATEWORDVALUE 10
#define INTERVAL_UPDATEWORDCOLOR 10
#define INTERVAL_UPDATEWORDPIXELS 10

// Indexes for special words
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
#define S 22
#define EIN 0

/* Includes */
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <coredecls.h>
#include <Adafruit_NeoPixel.h>

/* Structs */
typedef struct Word
{
  uint8_t needUpdate = 0;
  uint8_t address;
  uint8_t wordLength;
  uint16_t tarHue = 0;
  uint16_t curHue = 0;
  uint8_t tarSaturation = 0;
  uint8_t curSaturation = 0;
  uint8_t tarStatus = 0;
  uint8_t curValue = 0;
} Word;

/* Globals */

// previousMillis (pm)
unsigned long pm_updateTime = 0;
unsigned long pm_printTime = 0;
unsigned long pm_updateWordStatus = 0;
unsigned long pm_updateWordPixels = 0;
unsigned long pm_updateWordValue = 0;
unsigned long pm_updateValueFromAnalogInput = 0;
unsigned long pm_circleColor = 0;
unsigned long pm_updateWordColor = 0;

uint8_t firstFlow = 1;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER1, UTC_OFFSET, INTERVAL_UPDATENTP);
tm *tm; // stores Date and Time in human way

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_NUMBER, PIN_LED, NEO_GRB + NEO_KHZ800);

Word words[WORD_NUMBER];

uint8_t tarValue = 150;
uint8_t circleColorIndex = 0;

const uint16_t hues[COLOR_NUMBER] = {
  10922, 0, 6900, 10923, 21845, 32768, 43691, 49971, 54613
};
const uint8_t saturations[COLOR_NUMBER] = {
  70, 255, 255, 255, 255, 255, 255, 255, 115
};

// first digit=number of first led
// second digit=word length
const uint8_t addresses[WORD_NUMBER][2] = {
  {60, 3},  //Ein     0
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
  {99, 3},  //Uhr     21
  {63, 1}   //s       22
};

const uint8_t displayMode = 0;

void setup() {
  if (DEBUGLEVEL > 0) Serial.begin(115200);
  
  pixels.begin();
  pixels.clear();
  pixels.show();

  startNetwork();
  
  timeClient.begin();

  setWordAddresses();

  if (COLORMODE == 0) setWordSameColors();
  if (COLORMODE == 3) setWordDifferentColors();
}

void loop() {
  updateTime();
  printTime();
  updateWordStatus();

  if (USE_ANALOG_INPUT) updateValueFromAnalogInput();
  if (COLORMODE != 0 && COLORMODE != 3) updateColor();

  updateWordValue();
  updateWordColor();
  updateWordPixels();
  
  delay(1);
  ESP.wdtFeed();
  firstFlow = 0;
}
