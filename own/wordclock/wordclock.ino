
#define DEBUGLEVEL 2

#define PINLED D6
#define PINDHT D5
#define LEDNUMBER 110

uint8_t local_second;
uint8_t local_minute;
uint8_t local_hour;

unsigned long lastMillis_localTime;
#define updateInterval_localTime 1000
#define updateInterval_NTP 60000

#define millisPerSecond 1000
#define secondsPerMinute 60
#define minutesPerHour 60
#define hoursPerDay 24


void setup() {
  if (DEBUGLEVEL > 0) Serial.begin(115200);
  lastMillis_localTime = millis();

  //set local start time
  local_second = 30;
  local_minute = 59;
  local_hour = 23;
}

void loop() {
  if (DEBUGLEVEL > 2) Serial.println("New Loop");
  updateTime(); //Muss mindestens einmal pro Minute aufgerufen werden
  updateTimeNTP();
}

void updateTime() {
  const unsigned long currentMillis = millis();
  if (currentMillis - lastMillis_localTime <= updateInterval_localTime) {
    return;
  }
  
  local_second += (currentMillis - lastMillis_localTime) / millisPerSecond;
  if (local_second >= secondsPerMinute) {
    local_second -= secondsPerMinute;
    local_minute++;
  }
  if (local_minute >= minutesPerHour) {
    local_minute -= minutesPerHour;
    local_hour++;
  }
  if (local_hour >= hoursPerDay) {
    local_hour -= hoursPerDay;
  }
  
  lastMillis_localTime = currentMillis;
  printTime();
}

void printTime() {
  if (DEBUGLEVEL <= 1) {
    return;
  }
  Serial.print("Update Time; Now ");
  Serial.print(local_hour);
  Serial.print(":");
  Serial.print(local_minute);
  Serial.print(":");
  Serial.println(local_second);
}
}
