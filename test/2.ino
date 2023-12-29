#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <WiFiUdp.h>
#include <Ticker.h>
const char* ssid = "bobolili";
const char* password = "00000000";
const char* weather_host = "api.seniverse.com";
const char* weather_path = "/v3/weather/daily.json";
const char* weather_location = "hangzhou";
const char* weather_key = "S29tX0lKJkMZj5kk4";
// 定义时分秒
unsigned int h = 99, m = 99, s = 99;
// 监听本地UDP数据包端口
unsigned int localPort = 2390;
// NTP服务器IP地址
IPAddress timeServerIP;
// NTP服务器网址
const char* ntpServerName = "time.windows.com";
// NTP数据包数据长度
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
WiFiUDP udp;
// 创建一个需要定时调度的对象
Ticker ticker;
#define DHT_PIN    D6                      // 定义DHT传感器的数据引脚
#define OLED_CLK   D5                      // 定义OLED显示屏的时钟引脚
#define OLED_MOSI  D7                      // 定义OLED显示屏的数据引脚
#define OLED_DC    D2                      // 定义OLED显示屏的数据/命令选择引脚
#define OLED_CS    D0                      // 定义OLED显示屏的片选引脚
#define OLED_RESET D8                      // 定义OLED显示屏的复位引脚
WiFiClient client;
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC, OLED_RESET);
static const unsigned char PROGMEM logo_bmp[] ={ //0X00,0X01,0X80,0X00,0X40,0X00,
  0X00,0X00,0X00,0XF0,0X1F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X7F,0XF8,0X03,0X00,0X00,0X00,0X80,0X01,0X30,0X01,0X00,0X84,0X00,
  0X00,0X00,0XE0,0X01,0X00,0X1E,0X00,0XC0,0X60,0XB0,0XF1,0X33,0XF2,0X1F,0X84,0X01,
  0X00,0X00,0X18,0X00,0X00,0X70,0X00,0XC0,0X60,0X30,0X30,0X00,0X06,0X0C,0XFA,0X3F,
  0X00,0X00,0X06,0X00,0X00,0XC0,0X01,0X80,0XF1,0X10,0X30,0X00,0X64,0X06,0X02,0X00,
  0X00,0X80,0X01,0X64,0X15,0X00,0X03,0X80,0XF1,0X98,0X31,0X30,0XC0,0X01,0XF3,0X1F,
  0X00,0XC0,0X00,0X6E,0XB7,0X00,0X0C,0X80,0X91,0X98,0X31,0X30,0XF0,0X1F,0X03,0X00,
  0X00,0X30,0XA0,0XFB,0XBD,0X1B,0X18,0X80,0X99,0X99,0XF1,0XB3,0X13,0X91,0XF3,0X1F,
  0X00,0X18,0XF8,0X7F,0XEC,0X37,0X30,0X00,0X99,0X89,0X31,0X30,0XF2,0X9F,0X02,0X00,
  0X00,0X0C,0XF4,0X05,0X00,0X17,0X40,0X00,0X0B,0X89,0X31,0X30,0X12,0X11,0X02,0X00,
  0X00,0X06,0X65,0X00,0X00,0X1C,0X80,0X00,0X0F,0X8F,0X31,0X30,0XF2,0X1F,0XF2,0X1F,
  0X00,0X83,0X1F,0X00,0X00,0X00,0X01,0X01,0X0F,0X8F,0X31,0X30,0X12,0X11,0X12,0X10,
  0X80,0X81,0X06,0X00,0X00,0X80,0X04,0X03,0X06,0X86,0X31,0X30,0X12,0X11,0X12,0X10,
  0X80,0X80,0X03,0X00,0X00,0X80,0X0F,0X06,0X06,0X86,0X31,0X30,0X12,0X1F,0X12,0X10,
  0X40,0X00,0X00,0X00,0X00,0X00,0X0F,0X04,0X00,0X00,0X00,0X00,0X05,0X00,0XF2,0X1F,
  0X60,0X08,0X00,0X00,0X00,0X00,0X38,0X08,0X00,0X00,0X00,0X80,0XF8,0X3F,0X00,0X00,
  0X20,0X3C,0X00,0X00,0X03,0X00,0X78,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X30,0X2C,0X00,0XC0,0X07,0X00,0X70,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X10,0X3A,0X02,0XF8,0X3F,0X80,0X60,0X30,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X18,0X0E,0X03,0XFE,0XFF,0X81,0XE3,0X21,0X00,0X01,0XD0,0X3F,0X10,0X1A,0X42,0X00,
  0X08,0X8F,0X03,0XDE,0XEF,0X81,0X83,0X63,0X00,0X01,0X12,0X02,0XF8,0X7F,0X62,0X00,
  0X08,0X86,0X03,0XF0,0X1E,0X80,0XC3,0X43,0XF0,0X2F,0X12,0X02,0X48,0X22,0XE2,0X0F,
  0X84,0X87,0X1F,0XFE,0XFC,0XF0,0X43,0X41,0X00,0X31,0XD2,0X1F,0XC8,0XBF,0X2F,0X08,
  0X84,0XC3,0X0F,0XBF,0XF7,0XE1,0X87,0XC7,0X00,0X19,0X52,0X12,0X4C,0X22,0X52,0X08,
  0X04,0XC3,0X01,0XFF,0XEF,0X01,0X8E,0X87,0XFC,0X7F,0X52,0X12,0XCE,0X3F,0X82,0X08,
  0XC6,0XE3,0X04,0XFF,0XBF,0X43,0X0E,0X87,0X00,0X03,0X52,0X12,0X4A,0X22,0X02,0X09,
  0XC2,0XE1,0X07,0XFE,0XFF,0XC0,0X0F,0X81,0XE0,0X10,0X52,0X12,0X48,0X22,0X02,0X0E,
  0X02,0XC3,0X07,0XDF,0XF7,0X83,0X0F,0X8F,0X5C,0X0E,0X5A,0X12,0X08,0X10,0XCA,0X09,
  0X02,0XC1,0X83,0XE7,0XCF,0X03,0X0F,0X8F,0XC0,0X21,0X48,0X12,0XE8,0X7F,0X37,0X08,
  0XE2,0XE1,0X00,0XF8,0X3E,0X03,0X0C,0X83,0X41,0X20,0X04,0X1E,0X08,0X91,0X00,0X08,
  0XE2,0X61,0X06,0X7F,0XFF,0XC3,0X0C,0X0E,0XC1,0X1F,0X02,0X02,0X08,0X1E,0X80,0X0F,
  0XC2,0XE1,0X87,0XDF,0XE7,0XC3,0X0F,0X00,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0XC2,0XE1,0X87,0XE3,0X9F,0X83,0X0F,0X0E,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0XC2,0XC0,0X83,0XFF,0XFC,0X83,0X0F,0X0E,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0XC2,0XC1,0X89,0X3F,0XF8,0X27,0X07,0X88,0X11,0X31,0X40,0X00,0X00,0X00,0X00,0X00,
  0X42,0XC1,0X99,0X0F,0XE0,0X67,0X06,0X8F,0X21,0X1A,0X48,0X00,0X00,0X00,0X00,0X00,
  0XE2,0XC3,0X9D,0X01,0X00,0XE3,0X07,0X86,0XF9,0X7F,0X44,0X00,0X00,0X00,0X00,0X00,
  0X42,0XC3,0X1F,0X00,0X00,0XE0,0X07,0X88,0X08,0X40,0XFC,0X1F,0X00,0X00,0X00,0X00,
  0X02,0X80,0X4F,0XE0,0X1F,0XE4,0X03,0X80,0XE0,0X1F,0X42,0X00,0X00,0X00,0X00,0X00,
  0X06,0X00,0XCF,0XE0,0X1F,0XC6,0X03,0X80,0X00,0X18,0X41,0X00,0X00,0X00,0X00,0X00,
  0X04,0X00,0XCE,0X00,0X00,0XCE,0X00,0XC0,0X00,0X04,0X40,0X00,0X00,0X00,0X00,0X00,
  0X04,0X00,0XEC,0X0D,0X40,0XEE,0X00,0X40,0XF8,0X7F,0XFC,0X1F,0X00,0X00,0X00,0X00,
  0X0C,0X00,0XFC,0X1C,0X60,0XFE,0X00,0X40,0X00,0X02,0X40,0X00,0X00,0X00,0X00,0X00,
  0X08,0X00,0XFC,0X18,0X70,0X7E,0X00,0X60,0X00,0X02,0X40,0X00,0X00,0X00,0X00,0X00,
  0X08,0X00,0XF0,0X39,0X70,0X3E,0X18,0X20,0X00,0X02,0X40,0X00,0X00,0X00,0X00,0X00,
  0X18,0X40,0X80,0X3F,0XF0,0X07,0X1C,0X20,0XC0,0X03,0XFF,0X3F,0X00,0X00,0X00,0X00,
  0X10,0X40,0X80,0X7F,0XFC,0X03,0X3C,0X30,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X30,0XF0,0X01,0XFF,0XFF,0X03,0X7C,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X20,0XFC,0X01,0X8C,0XE7,0X00,0X38,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X40,0XFC,0X03,0X00,0X00,0X00,0X3C,0X0C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0XC0,0XE0,0X01,0X00,0X00,0X60,0X64,0X04,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X80,0XC1,0X10,0X00,0X00,0X60,0X00,0X02,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X01,0X10,0X00,0X00,0XC0,0XE0,0X7F,0XBE,0X20,0X7F,0XF8,0X7F,0XBA,0X63,0X3F,
  0X00,0X03,0X4C,0X00,0XF0,0XE0,0X80,0X03,0XA4,0XF0,0X41,0X90,0X24,0XAA,0X3A,0X21,
  0X00,0X04,0XCE,0X01,0X70,0XF8,0XC1,0X02,0XE4,0X17,0X7F,0XA0,0X94,0X36,0X23,0X21,
  0X00,0X08,0XFF,0X01,0XFC,0X78,0X61,0X02,0XA2,0X7C,0X41,0X80,0X84,0XB7,0X7B,0X3F,
  0X00,0X30,0X70,0X70,0XFC,0X20,0X30,0X02,0XB9,0X24,0X7F,0XF0,0X7F,0X6B,0X62,0X00,
  0X00,0X60,0X00,0X60,0XF0,0X00,0XF8,0X7F,0X80,0X24,0X00,0X10,0X00,0X22,0X70,0X7F,
  0X00,0XC0,0X00,0X7C,0XF8,0X00,0X06,0X02,0XBE,0XF4,0X91,0X10,0X00,0XFA,0XAB,0X08,
  0X00,0X00,0X03,0X7C,0XD8,0X80,0X03,0X02,0XA2,0X24,0X77,0X10,0X00,0X0A,0X2A,0X3F,
  0X00,0X00,0X0C,0X00,0X00,0XE0,0X00,0X02,0XA2,0X24,0X91,0X10,0X00,0XFD,0X2B,0X08,
  0X00,0X00,0X78,0X00,0X00,0X38,0X00,0X02,0X7E,0XA4,0X9D,0X08,0X00,0X0D,0X22,0X08,
  0X00,0X00,0XC0,0X07,0X80,0X0F,0XF0,0XFF,0X62,0X67,0XF3,0X08,0X80,0XF8,0XA3,0X7F,
  0X00,0X00,0X00,0XFC,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  };
const char* changeWeatherDayNames(const char* text_day){
  if(strcmp(text_day, "晴") == 0) {
          text_day = "sunny";
        }
        if(strcmp(text_day, "多云") == 0) {
            text_day = "cloudy";
        }
        if(strcmp(text_day, "阴") == 0) {
            text_day = "overcast";
        }
        if(strcmp(text_day, "雾") == 0) {
            text_day = "smoggy";
        }
        if(strcmp(text_day, "霾") == 0) {
            text_day = "haze";
        }
        if(strcmp(text_day, "冰雹") == 0) {
            text_day = "hail";
        }
        if(strcmp(text_day, "小雨") == 0) {
            text_day = "little rain";
        }
        if(strcmp(text_day, "阵雨") == 0) {
            text_day = "showers";
        }
        if(strcmp(text_day, "中雨") == 0) {
            text_day = "moderate rain";
        }
        if(strcmp(text_day, "大雨") == 0) {
            text_day = "heavy rain";
        }
        if(strcmp(text_day, "暴雨") == 0 ) {
            text_day = "torrential rain";
        }
        if(strcmp(text_day, "暴风雨") == 0) {
            text_day = "rain storm";
        }
        if(strcmp(text_day, "雨夹雪") == 0) {
            text_day = "rain and snow";
        }
        if(strcmp(text_day, "阵雪") == 0) {
            text_day = "showery snow";
        }
        if(strcmp(text_day, "小雪") == 0) {
            text_day = "light snow";
        }
        if(strcmp(text_day, "中雪") == 0) {
            text_day = "moderate snow";
        }
        if(strcmp(text_day, "大雪") == 0) {
            text_day = "heavy snow";
        }
        if(strcmp(text_day, "暴雪") == 0) {
            text_day = "blizzard";
        }
  return text_day;
  }
const char* changeWeatherNightNames(const char* text_night){
        if(strcmp(text_night, "晴") == 0) {
            text_night = "sunny";
        }
        if(strcmp(text_night, "多云") == 0) {
            text_night = "cloudy";
        }
        if(strcmp(text_night, "阴") == 0) {
            text_night = "overcast";
        }
        if(strcmp(text_night, "雾") == 0) {
            text_night = "smoggy";
        }
        if(strcmp(text_night, "霾") == 0) {
            text_night = "haze";
        }
        if(strcmp(text_night, "冰雹") == 0) {
            text_night = "hail";
        }
        if(strcmp(text_night, "小雨") == 0) {
            text_night = "little rain";
        }
        if(strcmp(text_night, "阵雨") == 0) {
            text_night = "showers";
        }
        if(strcmp(text_night, "中雨") == 0) {
            text_night = "moderate rain";
        }
        if(strcmp(text_night, "大雨") == 0) {
            text_night = "heavy rain";
        }
        if(strcmp(text_night, "暴雨") == 0 ) {
            text_night = "torrential rain";
        }
        if(strcmp(text_night, "暴风雨") == 0) {
            text_night = "rain storm";
        }
        if(strcmp(text_night, "雨夹雪") == 0) {
            text_night = "rain and snow";
        }
        if(strcmp(text_night, "阵雪") == 0) {
            text_night = "showery snow";
        }
        if(strcmp(text_night, "小雪") == 0) {
            text_night = "light snow";
        }
        if(strcmp(text_night, "中雪") == 0) {
            text_night = "moderate snow";
        }
        if(strcmp(text_night, "大雪") == 0) {
            text_night = "heavy snow";
        }
        if(strcmp(text_night, "暴雪") == 0) {
            text_night = "blizzard";
        }
  return text_night;
  }
void setup() {
  Serial.begin(115200);                     // 初始化串口通信，波特率为115200
  WiFi.mode(WIFI_STA);                      // 设置Wi-Fi模式为STA模式（客户端模式）
  WiFi.begin(ssid, password);                // 连接到Wi-Fi网络，使用给定的SSID和密码
  while (WiFi.status() != WL_CONNECTED) {    // 等待连接成功
    delay(1000);                             // 延迟1秒
    Serial.println("Connecting to WiFi..."); // 打印连接信息
  }
  Serial.println("Connected to WiFi");       // 连接成功后打印提示信息
  client.setTimeout(5000);                   // 设置客户端超时时间为5秒
  u8g2.begin();                              // 初始化U8g2显示库
  u8g2.setFont(u8g2_font_ncenB08_tf);         // 设置字体为ncenB08_tf
  u8g2.clearBuffer();                        // 清空显示缓冲区
  u8g2.drawXBMP(0, 0, 128, 64, logo_bmp);     // 在 (0,0) 位置绘制大小为 128x64 的位图
  u8g2.sendBuffer();                          // 将缓冲区的内容发送到显示屏上
  delay(2000);                               // 延迟2秒
}
void addtime() {
  // 更新时间逻辑
  if (s == 59) {                             // 如果秒为59
    s = 0;                                   // 秒置为0
    if (m == 59) {                           // 如果分钟为59
      m = 0;                                 // 分钟置为0
      if (h == 23) {                         // 如果小时为23
        h = 0;                               // 小时置为0
      } else {
        h++;                                // 否则小时加1
      }
    } else {
      m++;                                  // 否则分钟加1
    }
  } else {
    s++;                                  // 否则秒加1
  }
  Serial.print("当前时间为: ");              // 打印当前时间
  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  Serial.print(":");
  Serial.println(s);
}
/**
   获取十分秒信息
*/
void setTimes() {
  int cb = udp.parsePacket();                 // 解析Udp数据包
  if (!cb) {                                 // 如果包为空
    Serial.println("没有接收到任何的数据包!");   // 打印提示信息
  } else {                                   // 如果包不为空
    Serial.print("接收到的数据包的长度为: ");      // 打印包的长度
    Serial.println(cb);
    udp.read(packetBuffer, NTP_PACKET_SIZE);   // 读取UDP数据包中的数据
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);  // 获取高位字节
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);   // 获取低位字节
    unsigned long secsSince1900 = highWord << 16 | lowWord;              // 将高低位字节拼接成时间戳
    Serial.print("1900年格式标准的时间戳为:");                            // 打印原始时间戳
    Serial.println(secsSince1900);
    const unsigned long seventyYears = 2208988800UL;                     // 偏移量，用于转换为1970年格式的时间戳
    unsigned long epoch = secsSince1900 - seventyYears;                   // 偏移时间，转换为1970年格式的时间戳
    Serial.print("1970年格式标准的时间戳为:");                            // 打印转换后的时间戳
    Serial.println(epoch);
    h =  (epoch  % 86400L) / 3600 + 8;                                    // 计算时、分、秒
    m = (epoch % 3600) / 60;
    s = epoch % 60;
  }
}
/**
   发送ntp协议数据包
*/
void sendNTPpacket(IPAddress& address) {
  Serial.println("发送ntp数据包...");                                     // 打印提示信息
  memset(packetBuffer, 0, NTP_PACKET_SIZE);                              // 将数据包缓冲区清零
  packetBuffer[0] = 0b11100011;                                           // 设置请求头部分
  udp.beginPacket(address, 123);                                         // 配置远端ip地址和端口
  udp.write(packetBuffer, NTP_PACKET_SIZE);                              // 将数据写入发送缓冲区
  udp.endPacket();                                                        // 发送数据包
}
void loop() {
    if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(weather_host, 80) == 1) {
      Serial.println("Connection successful");
      Serial.print("设备IP地址: ");
      Serial.println(WiFi.localIP());
      Serial.println("开启UDP通信");
      udp.begin(localPort);
      Serial.print("本地端口为: ");
      Serial.println(udp.localPort());
      String url = String(weather_path) + "?location=" + weather_location + "&key=" + weather_key + "&language=zh-Hans&start=0&days=3";
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + weather_host + "\r\n" +
                   "Connection: close\r\n\r\n");
      Serial.println(url);
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println("Request timed out");
          client.stop();
          return;
        }
      }
      String response;
      while (client.available()) {
        response = client.readStringUntil('\r');
      }
      Serial.println(response);
      // 创建一个 JsonBuffer 用于解析 JSON 数据
      DynamicJsonDocument jsonBuffer(4096);
      // 解析 JSON 数据
      DeserializationError error = deserializeJson(jsonBuffer, response);
      if (error) {
        Serial.print(F("JSON 解析失败："));
        Serial.println(error.c_str());
        return;
      }
      // 获取 results 数组
      JsonArray results = jsonBuffer["results"];
      
      // 获取第一个结果对象
      JsonObject result = results[0];
      // 获取 daily 数组
      JsonArray daily = result["daily"];
      // 遍历未来三天的天气信息
      for (JsonVariant value : daily) {
        JsonObject day = value["date"];
        JsonObject weather = value["day"];
        // 获取日期
        const char* date = value["date"];
        // 获取天气信息
        const char* text_day = value["text_day"];
        const char* code_day = value["code_day"];
        const char* text_night = value["text_night"];
        const char* code_night = value["code_night"];
        const char* high = value["high"];
        const char* low = value["low"];
        //因为中文不能显示，所以要转换成英文
        text_day=changeWeatherDayNames(text_day);
        text_night=changeWeatherDayNames(text_night);
        // 显示天气信息
        Serial.println(date);
        Serial.println(text_day);
        Serial.println(code_day);
        Serial.println(text_night);
        Serial.println(code_night);
        Serial.println(high);
        Serial.println(low);
        Serial.println();
        //get a random server from the pool
        WiFi.hostByName(ntpServerName, timeServerIP);
        if (h == 99 || m == 99 || s == 99) {
            sendNTPpacket(timeServerIP); // send an NTP packet to a time server
            // 等一秒后获取结果
            delay(1000);
            setTimes();
            // 设置定时累加时间操作
            ticker.attach(1, addtime);
  }
        u8g2.clearBuffer();
        float codeday = value["code_day"];
        float codenight = value["code_night"];
        float highd = value["high"];
        float lowd = value["low"];
        u8g2.setCursor(0, 10);
        u8g2.print("d: ");
        u8g2.print(date);
        u8g2.setCursor(0, 20);
        u8g2.print("day: ");
        u8g2.print(text_day);
        u8g2.setCursor(0, 30);
        u8g2.print("night: ");
        u8g2.print(text_night);
        u8g2.setCursor(0, 40);
        u8g2.print("high: ");
        u8g2.print(highd);
        u8g2.setCursor(0, 50);
        u8g2.print("low: ");
        u8g2.print(lowd);
        u8g2.setCursor(0, 60);
        u8g2.print("t: ");
        u8g2.print(h);
        u8g2.print(":");
        u8g2.print(m);
        u8g2.print(":");
        u8g2.print(s);
        u8g2.sendBuffer();                        // 将缓冲区的内容发送到显示屏上
        delay(2000);
      }
    } else {
      Serial.println("Failed to connect to weather API");
      delay(5000);
    }
    client.stop();
  } else {
    Serial.println("WiFi disconnected");
  }
  delay(3000);
}
