
void setWordAddresses() {
  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    words[i].address = addresses[i][0];
    words[i].wordLength = addresses[i][1];
  }
}

void resetWordStatus() {
  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    words[i].tarStatus = 0;
  }
}

void updateWordStatus() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateWordStatus <= INTERVAL_UPDATEWORDSTATUS || firstFlow) {
    return;
  }
  pm_updateWordStatus = currentMillis;

  resetWordStatus();

  words[ES].tarStatus = 1;
  words[IST].tarStatus = 1;

  uint8_t index_hour = timeClient.getHours();
  if (timeClient.getMinutes() > 22 || timeClient.getMinutes() == 22 && timeClient.getMinutes() >= 30) {
    index_hour++;
  }
  if (index_hour == 0) index_hour = 12;
  if (index_hour > 12) index_hour -= 12;
  index_hour--;
  words[index_hour].tarStatus = 1;
  if (index_hour == N) {
    words[EI].tarStatus = 1;
    words[EI].tarHue = words[N].tarHue;
    words[EI].tarSaturation = words[N].tarSaturation;
    words[S].tarStatus = 1;
  }
  if (index_hour == ZW) {
    words[EI].tarStatus = 1;
    words[EI].tarHue = words[ZW].tarHue;
    words[EI].tarSaturation = words[ZW].tarSaturation;
  }
  if (index_hour == IEBEN) {
    words[S].tarStatus = 1;
  }
  
  if ((timeClient.getMinutes() > 57 || timeClient.getMinutes() == 57 && timeClient.getSeconds() >= 30) || (timeClient.getMinutes() < 2 || timeClient.getMinutes() == 2 && timeClient.getSeconds() < 30)) {
    words[UHR].tarStatus = 1;
    if (index_hour == N) {
      words[S].tarStatus = 0;
    }
  }

  if ((timeClient.getMinutes() > 2 || timeClient.getMinutes() == 2 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 7 || timeClient.getMinutes() == 7 && timeClient.getSeconds() < 30)) {
    words[FUENF].tarStatus = 1;
    words[NACH].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 7 || timeClient.getMinutes() == 7 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 12 || timeClient.getMinutes() == 12 && timeClient.getSeconds() < 30)) {
    words[ZEHN].tarStatus = 1;
    words[NACH].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 12 || timeClient.getMinutes() == 12 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 17 || timeClient.getMinutes() == 17 && timeClient.getSeconds() < 30)) {
    words[VIERTEL].tarStatus = 1;
    words[NACH].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 17 || timeClient.getMinutes() == 17 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 22 || timeClient.getMinutes() == 22 && timeClient.getSeconds() < 30)) {
    words[ZWANZIG].tarStatus = 1;
    words[NACH].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 22 || timeClient.getMinutes() == 22 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 27 || timeClient.getMinutes() == 27 && timeClient.getSeconds() < 30)) {
    words[FUENF].tarStatus = 1;
    words[VOR].tarStatus = 1;
    words[HALB].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 27 || timeClient.getMinutes() == 27 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 32 || timeClient.getMinutes() == 32 && timeClient.getSeconds() < 30)) {
    words[HALB].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 32 || timeClient.getMinutes() == 32 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 37 || timeClient.getMinutes() == 37 && timeClient.getSeconds() < 30)) {
    words[FUENF].tarStatus = 1;
    words[NACH].tarStatus = 1;
    words[HALB].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 37 || timeClient.getMinutes() == 37 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 42 || timeClient.getMinutes() == 42 && timeClient.getSeconds() < 30)) {
    words[ZWANZIG].tarStatus = 1;
    words[VOR].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 42 || timeClient.getMinutes() == 42 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 47 || timeClient.getMinutes() == 47 && timeClient.getSeconds() < 30)) {
    words[VIERTEL].tarStatus = 1;
    words[VOR].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 47 || timeClient.getMinutes() == 47 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 52 || timeClient.getMinutes() == 52 && timeClient.getSeconds() < 30)) {
    words[ZEHN].tarStatus = 1;
    words[VOR].tarStatus = 1;
  }

  if ((timeClient.getMinutes() > 52 || timeClient.getMinutes() == 52 && timeClient.getSeconds() >= 30) && (timeClient.getMinutes() < 57 || timeClient.getMinutes() == 57 && timeClient.getSeconds() < 30)) {
    words[FUENF].tarStatus = 1;
    words[VOR].tarStatus = 1;
  }
}

void updateWordValue() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateWordValue <= INTERVAL_UPDATEWORDVALUE && !firstFlow) {
    return;
  }
  pm_updateWordValue = currentMillis;

  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    if (words[i].curValue < words[i].tarStatus * tarValue) {
      if (words[i].curValue + VALUE_STEP > words[i].tarStatus * tarValue) {
        words[i].curValue = words[i].tarStatus * tarValue;
      } else {
        words[i].curValue += VALUE_STEP;
      }
      words[i].needUpdate = 1;
    }
    if (words[i].curValue > words[i].tarStatus * tarValue) {
      if (words[i].curValue - VALUE_STEP < words[i].tarStatus * tarValue) {
        words[i].curValue = words[i].tarStatus * tarValue;
      } else {
        words[i].curValue -= VALUE_STEP;
      }
      words[i].needUpdate = 1;
    }
  }
}

void updateWordPixels() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateWordPixels <= INTERVAL_UPDATEWORDPIXELS && !firstFlow) {
    return;
  }
  pm_updateWordPixels = currentMillis;

  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    if (words[i].needUpdate) {
      uint32_t rgbcolor = pixels.gamma32(pixels.ColorHSV(words[i].curHue, words[i].curSaturation, words[i].curValue));
      pixels.fill(rgbcolor, words[i].address, words[i].wordLength);
      words[i].needUpdate = 0;
      if (DEBUGLEVEL > 2) Serial.println(words[i].curValue);
    }
  }
  pixels.show();
}
