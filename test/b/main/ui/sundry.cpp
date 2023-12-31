#include "Arduino.h"
#include "config.h"

void checkFirstUse() {
  int value;
  EEPROM.get(4095, value);
  if (value == 31415926) {
    isFirstUse=false;
  } else {
    isFirstUse=true;
    EEPROM.put(4095,31415926);
  }
}