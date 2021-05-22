
void showNumber(uint16_t num, uint32_t col) {
  for (uint8_t pos = 0; pos <= 2;  pos++) {
    uint8_t stat[15] = {0};
    uint8_t addr[15];
    setDigitAddresses(addr, 8-4*pos);
    if (num != 0) {
      setDigitStatus(stat, num % 10, 8-4*pos);
      num /= 10;
    }
    for (uint8_t i = 0; i < 15; i++) {
      if (stat[i]) {
        pixels.setPixelColor(addr[i], col);
      } else {
        pixels.setPixelColor(addr[i], 0, 0, 0);
      }
    }
  }
  pixels.setPixelColor(95, col);
  pixels.show();
}

void setDigitAddresses(uint8_t *arr, uint8_t pos) {
  arr[0] = 43 - pos;
  arr[1] = 42 - pos;
  arr[2] = 41 - pos;
  arr[3] = 44 + pos;
  arr[4] = 45 + pos;
  arr[5] = 46 + pos;
  arr[6] = 65 - pos;
  arr[7] = 64 - pos;
  arr[8] = 63 - pos;
  arr[9] = 66 + pos;
  arr[10] = 67 + pos;
  arr[11] = 68 + pos;
  arr[12] = 87 - pos;
  arr[13] = 86 - pos;
  arr[14] = 85 - pos;
}

void setDigitStatus(uint8_t *arr, uint8_t digit, uint8_t pos) {
  arr[1] = 1;
  if (digit != 1) {
    arr[0] = 1;
    arr[2] = 1;
    arr[8] = 1;
    arr[14] = 1;
  }
  if (digit != 1 && digit != 2 && digit != 3 && digit != 7) {
    arr[3] = 1;
  }
  if (digit == 1) {
    arr[4] = 1;
    arr[10] = 1;
  }
  if (digit != 1 && digit != 5 && digit != 6) {
    arr[5] = 1;
  }
  if (digit != 1 && digit != 7) {
    arr[6] = 1;
  }
  if (digit != 7 && digit != 0) {
    arr[7] = 1;
  }
  if (digit == 2 || digit == 6 || digit == 8 || digit == 0) {
    arr[9] = 1;
  }
  if (digit != 1 && digit != 2) {
    arr[11] = 1;
  }
  if (digit != 1 && digit != 4 && digit != 7) {
    arr[12] = 1;
  }
  if (digit != 4 && digit != 7) {
    arr[13] = 1;
  }
}
