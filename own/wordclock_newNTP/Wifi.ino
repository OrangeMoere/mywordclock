
void startNetwork() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFISSID, WIFIPW);
  while (WiFi.status() != WL_CONNECTED) {
    if (DEBUGLEVEL > 0) Serial.print(".");
    //pixels.fill(pixels.Color(20,20,20));
    //pixels.show();
    delay(200);
    //pixels.clear();
    //pixels.show();
    //delay(100);
  }
  if (DEBUGLEVEL > 0) Serial.println("\nWiFi connected");
}
