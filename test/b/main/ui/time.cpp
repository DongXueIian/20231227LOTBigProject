#include "config.h"
#include "Arduino.h"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp3.aliyun.com");

tm* timeinfo=nullptr;

void NTPBegin() {
  // Initialize and synchronize time
  timeClient.begin();
  timeClient.update();

  // Wait for the time to be synchronized
  while (!timeClient.isTimeSet()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Time Synchronized");
}

void timeNetUpdate() {
  // Update time from NTP server
  timeClient.update();

  getLocalTime();
  // Print date and time
  Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n",
                timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

  // Print day of the week
  const char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  Serial.printf("Day of the week: %s\n", days[timeinfo->tm_wday]);
}

void getLocalTime() {
  // Get and print formatted time
  time_t rawTime = timeClient.getEpochTime();
  timeinfo = localtime(&rawTime);
  timeinfo->tm_year = timeinfo->tm_year + 1900;
  timeinfo->tm_mon = timeinfo->tm_mon + 1;
  timeinfo->tm_hour = timeinfo->tm_hour + 8;
}
