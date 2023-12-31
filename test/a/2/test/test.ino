#include "config.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  wifiSTABegin("bobolili", "00000000");
  NTPBegin();
}

int count = 0;
void loop() {

  // put your main code here, to run repeatedly:
  count++;
  if (count % 200000 == 0) {
    struct tm* timeinfo = timeNetUpdate();
    String* weatherData = getWeather();
    if (weatherData != nullptr) {
      Serial.println("Weather Data:");
      Serial.print("Text Day: ");
      Serial.println(weatherData[0]);
      Serial.print("Code Day: ");
      Serial.println(weatherData[1]);
      Serial.print("Text Night: ");
      Serial.println(weatherData[2]);
      Serial.print("Code Night: ");
      Serial.println(weatherData[3]);
      Serial.print("High: ");
      Serial.println(weatherData[4]);
      Serial.print("Low: ");
      Serial.println(weatherData[5]);
    } else {
      Serial.println("Failed to get weather data");
    }
    Serial.println(count);
    Serial.println("=====================================================");
  }
}
