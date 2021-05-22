
void updateColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_circleColor <= INTERVAL_CIRCLECOLOR && !firstFlow) {
    return;
  }
  pm_circleColor = currentMillis;

  if (COLORMODE == 1) setWordSameColors();
  if (COLORMODE == 2) {
    circleColorIndex = tm.tm_wday + 1;
    setWordSameColors();
  }
  if (COLORMODE == 4) setWordDifferentColors();
  
  if (circleColorIndex == COLOR_NUMBER - 1) {
    circleColorIndex = 0;
  } else {
    circleColorIndex++;
  }
}

void setWordSameColors() {
  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    words[i].tarHue = hues[circleColorIndex];
    words[i].tarSaturation = saturations[circleColorIndex];
  }
}

void setWordDifferentColors() {
  uint8_t offset_i = 4*circleColorIndex;
  for (uint8_t i = 0; i < 12; i++) {
    words[i].tarHue = hues[(i % 2 + offset_i) % COLOR_NUMBER];
    words[i].tarSaturation = saturations[(i % 2 + offset_i) % COLOR_NUMBER];
  }
  words[EIN].tarHue = hues[(0 + offset_i) % COLOR_NUMBER];
  words[EIN].tarSaturation = saturations[(0 + offset_i) % COLOR_NUMBER];
  words[ES].tarHue = hues[(2 + offset_i) % COLOR_NUMBER];
  words[ES].tarSaturation = saturations[(2 + offset_i) % COLOR_NUMBER];
  words[IST].tarHue = hues[(3 + offset_i) % COLOR_NUMBER];
  words[IST].tarSaturation = saturations[(3 + offset_i) % COLOR_NUMBER];
  words[FUENF].tarHue = hues[(4 + offset_i) % COLOR_NUMBER];
  words[FUENF].tarSaturation = saturations[(4 + offset_i) % COLOR_NUMBER];
  words[ZEHN].tarHue = hues[(5 + offset_i) % COLOR_NUMBER];
  words[ZEHN].tarSaturation = saturations[(5 + offset_i) % COLOR_NUMBER];
  words[VIERTEL].tarHue = hues[(4 + offset_i) % COLOR_NUMBER];
  words[VIERTEL].tarSaturation = saturations[(4 + offset_i) % COLOR_NUMBER];
  words[ZWANZIG].tarHue = hues[(5 + offset_i) % COLOR_NUMBER];
  words[ZWANZIG].tarSaturation = saturations[(5 + offset_i) % COLOR_NUMBER];
  words[HALB].tarHue = hues[(6 + offset_i) % COLOR_NUMBER];
  words[HALB].tarSaturation = saturations[(6 + offset_i) % COLOR_NUMBER];
  words[VOR].tarHue = hues[(7 + offset_i) % COLOR_NUMBER];
  words[VOR].tarSaturation = saturations[(7 + offset_i) % COLOR_NUMBER];
  words[NACH].tarHue = hues[(7 + offset_i) % COLOR_NUMBER];
  words[NACH].tarSaturation = saturations[(7 + offset_i) % COLOR_NUMBER];
  words[UHR].tarHue = hues[(8 + offset_i) % COLOR_NUMBER];
  words[UHR].tarSaturation = saturations[(8 + offset_i) % COLOR_NUMBER];
}

void updateWordColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - pm_updateWordColor <= INTERVAL_UPDATEWORDCOLOR && !firstFlow) {
    return;
  }
  pm_updateWordColor = currentMillis;

  for (uint8_t i = 0; i < WORD_NUMBER; i++) {
    if (words[i].tarStatus == 1) {
      if (words[i].curHue != words[i].tarHue) {
        if (words[i].curHue < words[i].tarHue && words[i].curHue + HUE_STEP > words[i].tarHue ||
        words[i].curHue > words[i].tarHue && words[i].curHue + HUE_STEP - 65536 > words[i].tarHue) {
          words[i].curHue = words[i].tarHue;
        } else {
          words[i].curHue += HUE_STEP;
        }
        words[i].needUpdate = 1;
      }
      if (words[i].curSaturation < words[i].tarSaturation) {
        if (words[i].curSaturation + SATURATION_STEP > words[i].tarSaturation) {
          words[i].curSaturation = words[i].tarSaturation;
        } else {
          words[i].curSaturation += SATURATION_STEP;
        }
        words[i].needUpdate = 1;
      }
      if (words[i].curSaturation > words[i].tarSaturation) {
        if (words[i].curSaturation - SATURATION_STEP < words[i].tarSaturation) {
          words[i].curSaturation = words[i].tarSaturation;
        } else {
          words[i].curSaturation -= SATURATION_STEP;
        }
        words[i].needUpdate = 1;
      }
    }
  }
}
