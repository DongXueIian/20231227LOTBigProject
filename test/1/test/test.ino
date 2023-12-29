// #include <functional>
#include "wifi.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

int count = 0;
void loop() {
  wifiSTABegin("bobolili", "00000000");
  // put your main code here, to run repeatedly:
  count++;
  if (count % 10000 == 0) {
    Serial.println(count);
    Serial.println("=====================================================");
  }
}
