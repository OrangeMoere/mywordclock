
void updateValueFromAnalogInput() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateValueFromAnalogInput <= INTERVAL_UPDATEVALUEFROMANALOGINPUT && !firstFlow) {
    return;
  }
  pm_updateValueFromAnalogInput = currentMillis;

  int32_t a0 = analogRead(A0); //liefert Werte von 1-1024
  if (DEBUGLEVEL > 1) Serial.println(a0);
  a0 -= 1;
  a0 = VALUE_MULTIPLIER * a0 + VALUE_OFFSET;
  a0 /= 4;
  if (a0 < 0) a0 = 0;
  if (a0 > 255) a0 = 255;
  tarValue = a0; //akzeptiert Werte von 0-255
}
