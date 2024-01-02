#include "Arduino.h"
#include "config.h"

bool isFirstUse=false;

void checkFirstUse() {
  long value;
  EEPROM.get(999, value);

  if (value == 31415926L) {
    Serial.println("is not first use 1111");
    isFirstUse = false;
  } else {
    isFirstUse = true;
    Serial.println("first use 0000");

    // 不需要手动调用 EEPROM.commit()，EEPROM.put() 会自动提交
    EEPROM.put(999, 31415926L);
    Serial.println("EEPROM write success");
  }
}
