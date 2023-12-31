#include "config.h"
#include "Arduino.h"

bool isConnectWiFi = false;

void wifiSTABegin(const char* ssid, const char* password) {
  if (isConnectWiFi == false) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    isConnectWiFi = true;
  }
  //  Serial.println("hello");
}

void disconnectWiFi() {
  WiFi.disconnect(true); // Disconnect from the current network and forget the credentials
  WiFi.mode(WIFI_OFF);   // Turn off the WiFi module
  isConnectWiFi=false;
}