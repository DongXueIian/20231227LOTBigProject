#include "config.h"
#include "Arduino.h"

const char* weather_host = "api.seniverse.com";
const char* weather_path = "/v3/weather/daily.json";
const char* weather_location = "hangzhou";
const char* weather_key = "S29tX0lKJkMZj5kk4";

// 监听本地UDP数据包端口
unsigned int localPort = 2390;

WiFiUDP udp;
WiFiClient client;

String* getWeather() {
  DynamicJsonDocument jsonBuffer(4096);
  DynamicJsonDocument result = updateWeather(jsonBuffer);
  if (result.isNull()) return nullptr;

  // 获取 results 数组
  JsonArray results = result["results"];
  // 获取第一个结果对象
  JsonObject dailyData = results[0]["daily"][0];

  // 动态分配 String 数组并返回指针
  String* weatherData = new String[6];
  weatherData[0] = dailyData["text_day"].as<String>();
  weatherData[1] = dailyData["code_day"].as<String>();
  weatherData[2] = dailyData["text_night"].as<String>();
  weatherData[3] = dailyData["code_night"].as<String>();
  weatherData[4] = dailyData["high"].as<String>();
  weatherData[5] = dailyData["low"].as<String>();

  return weatherData;
}
DynamicJsonDocument updateWeather(DynamicJsonDocument& jsonBuffer) {
  if (client.connect(weather_host, 80) == 1) {
    Serial.println("Connection successful");
    Serial.print("设备IP地址: ");
    Serial.println(WiFi.localIP());
    Serial.println("开启UDP通信");
    udp.begin(localPort);
    Serial.print("本地端口为: ");
    Serial.println(localPort);
    String url = String(weather_path) + "?location=" + weather_location + "&key=" + weather_key + "&language=zh-Hans&start=0&days=3";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + weather_host + "\r\n" + "Connection: close\r\n\r\n");
    Serial.println(url);
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Request timed out");
        client.stop();
        return DynamicJsonDocument(0);
      }
    }
    String response;
    while (client.available()) {
      response = client.readStringUntil('\r');
    }
    // Serial.println(response);
    // 创建一个 JsonBuffer 用于解析 JSON 数据
    DynamicJsonDocument jsonBuffer(4096);
    // 解析 JSON 数据
    DeserializationError error = deserializeJson(jsonBuffer, response);
    if (error) {
      Serial.print(F("JSON 解析失败："));
      Serial.println(error.c_str());
      return DynamicJsonDocument(0);
    }
    return jsonBuffer;
  } else {
    Serial.println("Failed to connect to weather API");
    delay(5000);
    return DynamicJsonDocument(0);
  }
}