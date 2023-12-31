#include "Arduino.h"
#include "config.h"

// 解码bool值，输入一个int，返回16个bool
bool* decodeBool(int x) {
  static bool boolArray[16];

  for (int i = 0; i < 16; ++i) {
    boolArray[i] = (x & (1 << i)) != 0;
  }

  return boolArray;
}

// 编码bool值，输入一个大小为16个bool值的数组，返回一个int
int codingBool(bool* bools) {
  int result = 0;

  for (int i = 0; i < 16; ++i) {
    if (bools[i]) {
      result |= (1 << i);
    }
  }

  return result;
}