#include "wifi.h"
#include "Arduino.h"
int getWifiSTA = 0;

void wifiSTABegin(const char* ssid, const char* password) {
  // if (getWifiSTA == 0) {
  //   WiFi.begin(ssid, password);
  //   while (WiFi.status() != WL_CONNECTED) {
  //     delay(500);
  //     Serial.print(".");
  //   }
  //   Serial.println("");
  //   Serial.println("WiFi connected.");
  //   Serial.println("IP address: ");
  //   Serial.println(WiFi.localIP());
  //   getWifiSTA = 1;
  // }
   Serial.println("hello");
}