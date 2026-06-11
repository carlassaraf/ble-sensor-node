#include "ui.h"
#include "screens.hpp"

void ScreenBLE::show() {
  _ui_screen_change(&ui_scrBLE, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_scrBLE_screen_init);
}

void ScreenBLE::update(float &temperature, float &humidity) {
  lv_label_set_text_fmt(ui_lblTempV, "%5.2f C", temperature);
  lv_label_set_text_fmt(ui_lblHumV, "%5.2f %%", humidity);
}

void ScreenBLE::hide() {

}

ScreenBLE::~ScreenBLE() {
  ui_scrBLE_screen_destroy();
}