#include"Arduino.h"
#include <ui.h>


//连接wifi
#include <WiFi.h>

extern bool isConnectWiFi;

void wifiSTABegin(const char* ssid, const char* password);
void disconnectWiFi();

//ntp服务器更新时间
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

extern tm * timeinfo;

void NTPBegin();
void timeNetUpdate();
void getLocalTime();

//心知天气获取时间
#include <ArduinoJson.h>
#include <WiFiUdp.h>

String* getWeather();
DynamicJsonDocument updateWeather(DynamicJsonDocument& jsonBuffer);

// clock.h
#include <EEPROM.h>

#define MAX_CLOCKS 10
#define CLOCK_EEPROM_SIZE 670
#define CLOCK_CHECK_OFFSET 0
#define CLOCK_DATA_OFFSET 10
#define CLOCK_HAS_RECORD 0
extern int currentClockLength;
struct clock {
  int id;
  int hour;
  int minute;
  char remark[60];
  bool active;
  bool everyday;
};
void clockSetup();
void writeIsClock(int index, bool value);
bool readIsClock(int index);
void printClock(const struct clock& clockData);
void loadIsClocks();
void writeClockData(int index, const struct clock& clockData);
void readClockData(int index, struct clock& clockData);
void reinstallClock();
void addClock(int hour, int minute, String remake, bool active, bool everyday);
void delClock(int clockId);
void updateClock(int index,int hour, int minute, String remake, bool active, bool everyday);
struct clock* getClocks();
struct clock getNewestClocks();
#ifdef _SQUARELINE_PROJECT_UI_H
void addClockPageSetup();
void clockPagePrint();
void ui_myEvent_lastPage(lv_event_t* e);
void ui_myEvent_nextPage(lv_event_t* e);
void ui_myEvent_sc3CheckBox0(lv_event_t * e);
void ui_myEvent_sc3ImageButton0(lv_event_t * e);
void ui_myEvent_sc3CheckBox1(lv_event_t * e);
void ui_myEvent_sc3ImageButton1(lv_event_t * e);
void ui_myEvent_sc3CheckBox2(lv_event_t * e);
void ui_myEvent_sc3ImageButton2(lv_event_t * e);
void ui_myEvent_minuteText(lv_event_t * e);
void ui_myEvent_hourText(lv_event_t * e);
void ui_myEvent_remakeText(lv_event_t * e);
void ui_myEvent_sc4Save(lv_event_t* e);
#endif  

// sundry杂项
extern bool isFirstUse;
void checkFirstUse();