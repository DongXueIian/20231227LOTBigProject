#include <lvgl.h>
#include <demos/lv_demos.h>
#include <examples/lv_examples.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include "touch.h"
// #include "wifi.h"


static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[screenWidth*30];
TFT_eSPI my_lcd = TFT_eSPI();

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  my_lcd.setAddrWindow(area->x1, area->y1, w, h);
  my_lcd.pushColors((uint16_t *)&color_p->full, w * h, true);
  lv_disp_flush_ready(disp);
}

/* Reading input device (simulated encoder here) */
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch_has_signal()) {
    if (touch_touched()) {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    } else if (touch_released()) {
      data->state = LV_INDEV_STATE_REL;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */
  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  my_lcd.init();
  my_lcd.setRotation(1);
  touch_init(my_lcd.width(), my_lcd.height(), my_lcd.getRotation());
  lv_init();
  delay(10);

#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  lv_disp_draw_buf_init(&disp_buf, buf, NULL, screenWidth * 30);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = my_lcd.width();
  disp_drv.ver_res = my_lcd.height();
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  /* Create simple label */
  //lv_obj_t *label = lv_label_create( lv_scr_act() );
  //lv_label_set_text( label, LVGL_Arduino.c_str() );
  // lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );

  // uncomment one of these demos
  ui_init();
  // wifiSTABegin("bobolili","00000000");
}
long lastTime = 0;
void loop() {
  lv_task_handler(); /* let the GUI do its work */
  // if(millis()-lastTime>1000){
  //   lastTime=millis();
  //   sc1_Animation(ui_Panel2, 0);
  // }
  delay(5);
  // lv_label_set_text(ui_sc1L1, " 2:12");
}

String serialEvent(String serialStr) {
  // 检查是否有可用的串口数据
  while (Serial.available() > 0) {
    // 读取串口数据并追加到字符串
    char incomingChar = Serial.read();
    serialStr += incomingChar;
  }
  return serialStr;
}
