#include "wifi.h"
int wifiSTABegin(ssid,password){
    WiFi.begin(ssid, password);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}