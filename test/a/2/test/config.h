#include"Arduino.h"

//连接wifi
#include <WiFi.h>

void wifiSTABegin(const char* ssid, const char* password);
void disconnectWiFi();

//ntp服务器更新时间
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

void NTPBegin();
tm *timeNetUpdate();

//心知天气获取时间
#include <ArduinoJson.h>
#include <WiFiUdp.h>

String* getWeather();
DynamicJsonDocument updateWeather(DynamicJsonDocument& jsonBuffer);