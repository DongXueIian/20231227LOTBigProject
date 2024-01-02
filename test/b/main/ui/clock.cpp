#include "Arduino.h"
#include "config.h"

int currentClockLength = 0;
int lastTimeClock = 0;
bool isClocks[MAX_CLOCKS];
int currentClock = 0;

void clockSetup() {
  if (isFirstUse == true) {
    reinstallClock();
  }
  loadIsClocks();
#ifdef _SQUARELINE_PROJECT_UI_H
  addClockPageSetup();
  clockPagePrint();
#endif
}

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
    EEPROM.get(CLOCK_CHECK_OFFSET + i, isClocks[i]);
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

void printClock(const struct clock& clockData) {
  Serial.print("ID: ");
  Serial.println(clockData.id);
  
  Serial.print("Time: ");
  Serial.print(clockData.hour);
  Serial.print(":");
  Serial.println(clockData.minute);

  Serial.print("Remark: ");
  Serial.println(clockData.remark);

  Serial.print("Active: ");
  Serial.println(clockData.active ? "Yes" : "No");

  Serial.print("Everyday: ");
  Serial.println(clockData.everyday ? "Yes" : "No");

  Serial.println("==============================================="); // 添加一个空行作为分隔
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

void updateClock(int index, int hour, int minute, String remake, bool active, bool everyday) {
  if (index >= 0 && index < MAX_CLOCKS && readIsClock(index)) {
    struct clock updatedClock;
    updatedClock.id = index;
    updatedClock.hour = hour;
    updatedClock.minute = minute;
    remake.toCharArray(updatedClock.remark, sizeof(updatedClock.remark));
    updatedClock.active = active;
    updatedClock.everyday = everyday;

    writeClockData(index, updatedClock);
    // struct clock* clocks = getClocks();
    // printClock(clocks[index]);
    // free(clocks);
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

    // 对闹钟数组进行按时间排序（冒泡排序）
    for (int i = 0; i < validClocksCount - 1; i++) {
        for (int j = 0; j < validClocksCount - i - 1; j++) {
            int timeA = clocks[j].hour * 60 + clocks[j].minute;
            int timeB = clocks[j + 1].hour * 60 + clocks[j + 1].minute;

            if (timeA > timeB) {
                // 交换两个元素
                struct clock temp = clocks[j];
                clocks[j] = clocks[j + 1];
                clocks[j + 1] = temp;
            }
        }
    }
    
    currentClockLength = currentIndex;
    for(int i=0;i<currentClockLength;i++){
      printClock(clocks[i]);
    }
    return clocks;
}


struct clock getNewestClocks() {
  struct clock newestClock;
  newestClock.id = -1;  // 或者使用其他合适的默认值
  int newestTime = 999999, currentTime = 0;
  struct clock* clocks = getClocks();
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
  free(clocks);
  return newestClock;
}


#ifdef _SQUARELINE_PROJECT_UI_H
#pragma message("success include ui.h")
bool isAddClockPageSetup = false;
void addClockPageSetup() {
  if (!isAddClockPageSetup) {
    lv_obj_add_event_cb(ui_minuteText, ui_myEvent_minuteText, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ui_hourText, ui_myEvent_hourText, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ui_remakeText, ui_myEvent_remakeText, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ui_okButton, ui_myEvent_sc4Save, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_nextPage, ui_myEvent_nextPage, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_lastPage, ui_myEvent_lastPage, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_event_cb(ui_exerydayCheck, ui_exerydayCheck_testEvent, LV_EVENT_ALL, NULL);
    isAddClockPageSetup = true;
  }
}
const int num_panels = MAX_CLOCKS + 4;
lv_obj_t* ui_Sc3Panel[num_panels];
lv_obj_t* ui_Label_Sc3Time[num_panels];
lv_obj_t* ui_Label_Sc3Task[num_panels];
lv_obj_t* ui_Sc3Switch[num_panels];
lv_obj_t* ui_Sc3ImageButton[num_panels];
void clockPagePrint() {
  for (int i = 0; i < num_panels; i++) {
    if (ui_Sc3Panel[i] != NULL) {
      Serial.println("操作 lv_obj_del(ui_Sc3Panel["+String(i)+"])");
      lv_obj_del(ui_Sc3Panel[i]);
      ui_Sc3Panel[i] = NULL;  // 可以将指针设置为NULL，防止被误用
      ui_Label_Sc3Time[i] = NULL;
      ui_Label_Sc3Task[i] = NULL;
      ui_Sc3Switch[i] = NULL;
      ui_Sc3ImageButton[i] = NULL;
    }
  }
  delay(5);
  // for(int i=0;i<num_panels;i+=5){
  //   for(int j=0;j<5;j++){
  //     if(ui_Sc3Panel[i+j] == NULL)Serial.printf("NULL ");
  //     else Serial.printf("0000 ");
  //   }
  //   Serial.println();
  // }
  struct clock* clocks = getClocks();
  if (clocks != NULL) {
    for (int i = currentClock; i < currentClock + 3 && i < currentClockLength; i++) {
      Serial.println("操作 ui_Sc3Panel["+String(i)+"] = lv_obj_create(ui_Screen3);");
      ui_Sc3Panel[i] = lv_obj_create(ui_Screen3);
      lv_obj_set_width(ui_Sc3Panel[i], 260);
      lv_obj_set_height(ui_Sc3Panel[i], 40);
      lv_obj_set_x(ui_Sc3Panel[i], 0);
      lv_obj_set_y(ui_Sc3Panel[i], 30 + (i-currentClock) * 50);
      lv_obj_set_align(ui_Sc3Panel[i], LV_ALIGN_TOP_MID);
      lv_obj_clear_flag(ui_Sc3Panel[i], LV_OBJ_FLAG_SCROLLABLE);  /// Flags


      ui_Label_Sc3Time[i] = lv_label_create(ui_Sc3Panel[i]);
      lv_obj_set_width(ui_Label_Sc3Time[i], LV_SIZE_CONTENT);   /// 1
      lv_obj_set_height(ui_Label_Sc3Time[i], LV_SIZE_CONTENT);  /// 1
      lv_obj_set_x(ui_Label_Sc3Time[i], 25);
      lv_obj_set_y(ui_Label_Sc3Time[i], 0);
      lv_obj_set_align(ui_Label_Sc3Time[i], LV_ALIGN_LEFT_MID);
      lv_label_set_text(ui_Label_Sc3Time[i], (String(clocks[i].hour) + ":" + String(clocks[i].minute)).c_str());
      lv_obj_set_style_text_font(ui_Label_Sc3Time[i], &ui_font_sc1d, LV_PART_MAIN | LV_STATE_DEFAULT);

      ui_Label_Sc3Task[i] = lv_label_create(ui_Sc3Panel[i]);
      lv_obj_set_width(ui_Label_Sc3Task[i], LV_SIZE_CONTENT);   /// 1
      lv_obj_set_height(ui_Label_Sc3Task[i], LV_SIZE_CONTENT);  /// 1
      lv_obj_set_x(ui_Label_Sc3Task[i], 75);
      lv_obj_set_y(ui_Label_Sc3Task[i], 0);
      lv_obj_set_align(ui_Label_Sc3Task[i], LV_ALIGN_LEFT_MID);
      lv_label_set_text(ui_Label_Sc3Task[i], clocks[i].remark);
      lv_label_set_long_mode(ui_Label_Sc3Task[i], LV_LABEL_LONG_SCROLL_CIRCULAR);
      lv_obj_set_style_text_font(ui_Label_Sc3Task[i], &ui_font_sc1d, LV_PART_MAIN | LV_STATE_DEFAULT);

      ui_Sc3Switch[i] = lv_switch_create(ui_Sc3Panel[i]);
      lv_obj_set_width(ui_Sc3Switch[i], 40);
      lv_obj_set_height(ui_Sc3Switch[i], 20);
      lv_obj_set_x(ui_Sc3Switch[i], 10);
      lv_obj_set_y(ui_Sc3Switch[i], 0);
      lv_obj_set_align(ui_Sc3Switch[i], LV_ALIGN_RIGHT_MID);
      if(clocks[i].active){
        lv_obj_add_state(ui_Sc3Switch[i], LV_STATE_CHECKED);       /// States
      }

      ui_Sc3ImageButton[i] = lv_imgbtn_create(ui_Sc3Panel[i]);
      lv_imgbtn_set_src(ui_Sc3ImageButton[i], LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_1061045036, NULL);
      lv_obj_set_height(ui_Sc3ImageButton[i], 30);
      lv_obj_set_width(ui_Sc3ImageButton[i], LV_SIZE_CONTENT);  /// 1
      lv_obj_set_x(ui_Sc3ImageButton[i], -10);
      lv_obj_set_y(ui_Sc3ImageButton[i], 2);
      lv_obj_set_align(ui_Sc3ImageButton[i], LV_ALIGN_LEFT_MID);

    }
    Serial.println("操作 ui_Sc3Panel事件绑定");
    if (ui_Sc3Switch[0+currentClock] != NULL && ui_Sc3ImageButton[0+currentClock] != NULL) {
      lv_obj_add_event_cb(ui_Sc3Switch[0+currentClock], ui_myEvent_sc3CheckBox0, LV_EVENT_VALUE_CHANGED, NULL);
      lv_obj_add_event_cb(ui_Sc3ImageButton[0+currentClock], ui_myEvent_sc3ImageButton0, LV_EVENT_CLICKED, NULL);
      Serial.println("操作 ui_Sc3Panel0事件绑定");
    }
    if (ui_Sc3Switch[1+currentClock] != NULL && ui_Sc3ImageButton[1+currentClock] != NULL) {
      lv_obj_add_event_cb(ui_Sc3Switch[1+currentClock], ui_myEvent_sc3CheckBox1, LV_EVENT_VALUE_CHANGED, NULL);
      lv_obj_add_event_cb(ui_Sc3ImageButton[1+currentClock], ui_myEvent_sc3ImageButton1, LV_EVENT_CLICKED, NULL);
      Serial.println("操作 ui_Sc3Panel1事件绑定");
    }
    if (ui_Sc3Switch[2+currentClock] != NULL && ui_Sc3ImageButton[2+currentClock] != NULL) {
      lv_obj_add_event_cb(ui_Sc3Switch[2+currentClock], ui_myEvent_sc3CheckBox2, LV_EVENT_VALUE_CHANGED, NULL);
      lv_obj_add_event_cb(ui_Sc3ImageButton[2+currentClock], ui_myEvent_sc3ImageButton2, LV_EVENT_CLICKED, NULL);
      Serial.println("操作 ui_Sc3Panel2事件绑定");
    }
    Serial.println("操作 clockPagePrint释放clocks");
    // 释放动态分配的内存
    free(clocks);
  } else {
    Serial.println("Failed to get clocks");
  }
}

void ui_myEvent_lastPage(lv_event_t* e) {
  if (currentClock > 0) {
    currentClock -= 3;
    Serial.println("当前currentClock：");
    Serial.println(currentClock);
    Serial.println("上一页");
    clockPagePrint();
  }
}

void ui_myEvent_nextPage(lv_event_t* e) {
  if (currentClock < MAX_CLOCKS  + 2) {
    currentClock += 3;
    Serial.println("当前currentClock：");
    Serial.println(currentClock);
    Serial.println("下一页");
    clockPagePrint();
  }
}

void ui_myEvent_sc3CheckBox0(lv_event_t* e) {
  Serial.println("修改对象:0");
  Serial.println("当前currentClock：");
  Serial.println(currentClock);
  lv_obj_t* obj = lv_event_get_target(e);
  // 获取对应的 clock 对象
  struct clock* clocks = getClocks();
  if (clocks != NULL) {
    // 更新对应 clock 对象的状态
    updateClock(clocks[0 + currentClock].id, clocks[0 + currentClock].hour, clocks[0 + currentClock].minute, clocks[0 + currentClock].remark, lv_obj_has_state(obj, LV_STATE_CHECKED), clocks[0 + currentClock].everyday);
    // 释放动态分配的内存
    free(clocks);
  } else {
    Serial.println("Failed to get clocks or invalid switch index");
  }
}

void ui_myEvent_sc3ImageButton0(lv_event_t* e) {
  Serial.println("删除对象:0");
  Serial.println("当前currentClock：");
  Serial.println(currentClock);
  struct clock* clocks = getClocks();
  if (clocks != NULL) {
    delClock(clocks[0 + currentClock].id);
    // 释放动态分配的内存
    free(clocks);
  } else {
    Serial.println("Failed to get clocks or invalid switch index");
  }
  clockPagePrint();
}

void ui_myEvent_sc3CheckBox1(lv_event_t* e) {
  Serial.println("修改对象:1");
  Serial.println("当前currentClock：");
  Serial.println(currentClock);
  lv_obj_t* obj = lv_event_get_target(e);
  // 获取对应的 clock 对象
  struct clock* clocks = getClocks();
  if (clocks != NULL) {
    // 更新对应 clock 对象的状态
    updateClock(clocks[1 + currentClock].id, clocks[1 + currentClock].hour, clocks[1 + currentClock].minute, clocks[1 + currentClock].remark, lv_obj_has_state(obj, LV_STATE_CHECKED), clocks[1 + currentClock].everyday);
    // 释放动态分配的内存
    free(clocks);
  } else {
    Serial.println("Failed to get clocks or invalid switch index");
  }
}

void ui_myEvent_sc3ImageButton1(lv_event_t* e) {
  Serial.println("删除对象:1");
  Serial.println("当前currentClock：");
  Serial.println(currentClock);
  struct clock* clocks = getClocks();
  if (clocks != NULL) {
    delClock(clocks[1 + currentClock].id);
    // 释放动态分配的内存
    free(clocks);
  } else {
    Serial.println("Failed to get clocks or invalid switch index");
  }
  clockPagePrint();
}

void ui_myEvent_sc3CheckBox2(lv_event_t* e) {
  Serial.println("修改对象:2");
  Serial.println("当前currentClock：");
  Serial.println(currentClock);
  lv_obj_t* obj = lv_event_get_target(e);
  // 获取对应的 clock 对象
  struct clock* clocks = getClocks();
  if (clocks != NULL) {
    // 更新对应 clock 对象的状态
    updateClock(clocks[2 + currentClock].id, clocks[2 + currentClock].hour, clocks[2 + currentClock].minute, clocks[2 + currentClock].remark, lv_obj_has_state(obj, LV_STATE_CHECKED), clocks[2 + currentClock].everyday);
    // 释放动态分配的内存
    free(clocks);
  } else {
    Serial.println("Failed to get clocks or invalid switch index");
  }
}

void ui_myEvent_sc3ImageButton2(lv_event_t* e) {
  Serial.println("删除对象:2");
  Serial.println("当前currentClock：");
  Serial.println(currentClock);
  struct clock* clocks = getClocks();
  if (clocks != NULL) {
    delClock(clocks[2 + currentClock].id);
    // 释放动态分配的内存
    free(clocks);
  } else {
    Serial.println("Failed to get clocks or invalid switch index");
  }
  clockPagePrint();
}

void ui_myEvent_minuteText(lv_event_t* e) {
  lv_keyboard_set_textarea(ui_Keyboard1, ui_minuteText);
}

void ui_myEvent_hourText(lv_event_t* e) {
  lv_keyboard_set_textarea(ui_Keyboard1, ui_hourText);
}

void ui_myEvent_remakeText(lv_event_t* e) {
  lv_keyboard_set_textarea(ui_Keyboard1, ui_remakeText);
}

void ui_myEvent_sc4Save(lv_event_t* e) {
  if (millis() - lastTimeClock > 1000) {
    Serial.println("ui_myEvent_sc4Save be used");
    addClock(atoi(lv_textarea_get_text(ui_hourText)),
             atoi(lv_textarea_get_text(ui_minuteText)),
             String(lv_textarea_get_text(ui_remakeText)),
             true,
             lv_obj_get_state(ui_exerydayCheck));
    lastTimeClock = millis();
  }
  clockPagePrint();
  delay(5);
  _ui_screen_change(&ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_Screen3_screen_init);
}


#endif