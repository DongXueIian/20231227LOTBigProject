#include "Arduino.h"
#include "config.h"

bool isClocks[MAX_CLOCKS];

void writeIsClock(int index, bool value) {
  isClocks[index] = value;
  EEPROM.write(CLOCK_CHECK_OFFSET + index, value);
  EEPROM.commit();
}

bool readIsClock(int index) {
  return isClocks[index];
}

void loadIsClocks() {
  for (int i = 0; i < MAX_CLOCKS; ++i) {
    isClocks[i] = readIsClock(i);
  }
}

void writeClockData(int index, const struct clock& clockData) {
  EEPROM.put(CLOCK_DATA_OFFSET + index * sizeof(struct clock), clockData);
  EEPROM.commit();
}

void readClockData(int index, struct clock& clockData) {
  EEPROM.get(CLOCK_DATA_OFFSET + index * sizeof(struct clock), clockData);
}

void reinstallClock() {

  // 初始化闹钟检查标志
  for (int i = 0; i < MAX_CLOCKS; ++i) {
    isClocks[i] = false;
    writeIsClock(i, false);
  }
}

void addClock(int hour, int minute, String remake, bool active, bool everyday) {
  struct clock newClock;
  int newId = -1;

  // 查找空闲位置
  for (int i = 0; i < MAX_CLOCKS; ++i) {
    if (!readIsClock(i)) {
      newId = i;
      break;
    }
  }

  // 如果找到空闲位置，写入新闹钟
  if (newId != -1) {
    newClock.id = newId;
    newClock.hour = hour;
    newClock.minute = minute;
    remake.toCharArray(newClock.remark, sizeof(newClock.remark));
    newClock.active = active;
    newClock.everyday = everyday;

    writeClockData(newId, newClock);
    writeIsClock(newId, true);
  }
}

void delClock(int clockId) {
  if (clockId >= 0 && clockId < MAX_CLOCKS) {
    writeIsClock(clockId, false);
  }
}

struct clock* getClocks() {
  int validClocksCount = 0;

  // 计算有效的闹钟数量
  for (int i = 0; i < MAX_CLOCKS; ++i) {
    if (readIsClock(i)) {
      validClocksCount++;
    }
  }

  // 分配动态内存
  struct clock* clocks = (struct clock*)malloc(validClocksCount * sizeof(struct clock));

  if (clocks == NULL) {
    // 处理内存分配失败的情况
    return NULL;
  }

  // 将有效的闹钟数据复制到动态分配的数组中
  int currentIndex = 0;
  for (int i = 0; i < MAX_CLOCKS; ++i) {
    if (readIsClock(i)) {
      readClockData(i, clocks[currentIndex]);
      currentIndex++;
    }
  }

  currentClockLength=currentIndex;
  return clocks;
}

struct clock getNewestClocks() {
  struct clock newestClock;
  int newestTime = 999999,currentTime =0;
  struct clock* clocks=getClocks();
  for (int i = 0; i < currentClockLength; ++i) {
    if (clocks[i].active) {
      // 计算时间戳或其他条件来判断最近的闹钟
      currentTime = clocks[i].hour * 60 + clocks[i].minute;
      if (currentTime < newestTime) {
        newestTime = currentTime;
        newestClock = clocks[i];
      }
    }
  }

  return newestClock;
}
