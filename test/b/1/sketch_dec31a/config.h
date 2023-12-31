#include "Arduino.h"

#include <bitset>
bool* decodeBool(int x);
int codingBool(bool* bools);

// clock.h
#include <EEPROM.h>

#define MAX_CLOCKS 10
#define CLOCK_EEPROM_SIZE 670
#define CLOCK_CHECK_OFFSET 0
#define CLOCK_DATA_OFFSET 10
extern int currentClockLength;
struct clock {
  int id;
  int hour;
  int minute;
  char remark[60];
  bool active;
  bool everyday;
};