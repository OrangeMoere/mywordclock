

#define WIFISSID "nuresp"
#define WIFIPW  "esp8266!"

#define DEBUGLEVEL 2

/* Configuration of Pins */
#define PINLED D6
#define PINDHT D5

/* Configuration of NTP */
#define NTP_SERVER1 "192.168.178.1" 
#define NTP_SERVER2 "de.pool.ntp.org"
#define TZ "CET-1CEST,M3.5.0/2:00,M10.5.0/3:00"

/* Update Intervals */
#define INTERVAL_UPDATETIME 1000
#define INTERVAL_PRINTTIME 1000
#define INTERVAL_UPDATENTP 10*1000

/* previousMillis (pm) */
unsigned long pm_updateTime = 0;
unsigned long pm_printTime = 0;

/* Necessary Includes */
#include <ESP8266WiFi.h>            // we need wifi to get internet access
#include <time.h>                   // time() ctime()
#include <coredecls.h>

/* Globals */
tm tm;                              // the structure tm holds time information in a more convient way

void setup() {
  if (DEBUGLEVEL > 0) Serial.begin(115200);

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
}

void loop() {
  updateTime();
  printTime();
  
}

void time_is_set() { //Diese Funktion wird als Rückruf festgelegt, wenn Zeitdaten abgerufen wurden.
  if (DEBUGLEVEL > 0) Serial.println("********* NTP Server Timestap Synchronisation  *********");
}

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 () { // SNTP-Aktualisierungsverzögerung ändern, ohne diese Funktion wird jede Stunde die Zeit geholt.
  return INTERVAL_UPDATENTP;
}

void updateTime() {
  unsigned long currentMillis = millis();
  if(currentMillis - pm_updateTime <= INTERVAL_UPDATETIME) {
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
  if(currentMillis - pm_printTime <= INTERVAL_PRINTTIME) {
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
