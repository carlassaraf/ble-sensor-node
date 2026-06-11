#include "ui.h"
#include "screens.hpp"

void ScreenBLE::show() {
  _ui_screen_change(&ui_scrBLE, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_scrBLE_screen_init);
}

void ScreenBLE::update(float &temperature, float &humidity) {
  lv_label_set_text_fmt(ui_lblTempV, "%02d.%02d C", (int)temperature, (int)(100 * (temperature - (int)temperature)));
  lv_label_set_text_fmt(ui_lblHumV, "%02d.%02d %%", (int)humidity, (int)(100 * (humidity - (int)humidity)));
}

void ScreenBLE::hide() {

}

ScreenBLE::~ScreenBLE() {
  ui_scrBLE_screen_destroy();
}