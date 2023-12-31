#include "config.h"
#include "Arduino.h"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp3.aliyun.com");

void NTPBegin(){
      // Initialize and synchronize time
  timeClient.begin();
  timeClient.update();

  // Wait for the time to be synchronized
  while (!timeClient.isTimeSet())
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Time Synchronized");
}

tm *timeNetUpdate(){
    // Update time from NTP server
  timeClient.update();

  // Get and print formatted time
  time_t rawTime = timeClient.getEpochTime();
  struct tm *timeinfo;
  timeinfo = localtime(&rawTime);
  timeinfo->tm_year=timeinfo->tm_year+1900;
  timeinfo->tm_mon=timeinfo->tm_mon+1;
  timeinfo->tm_hour=timeinfo->tm_hour+8;
  // Print date and time
  Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n",
                timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

  // Print day of the week
  const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  Serial.printf("Day of the week: %s\n", days[timeinfo->tm_wday]);
  return timeinfo;
}
