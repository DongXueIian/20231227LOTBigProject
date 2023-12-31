#include "config.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  EEPROM.begin(4096);  // 初始化EEPROM库，设置Flash存储大小

  int eepromSize = EEPROM.length();
  Serial.print("EEPROM Size: ");
  Serial.print(eepromSize);
  Serial.println(" bytes");
}

void loop() {
  // put your main code here, to run repeatedly:
}
