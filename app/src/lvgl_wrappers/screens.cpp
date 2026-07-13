#include "ui/ui.h"
#include "screens.hpp"

void ScreenBLE::show() {
  _ui_screen_change(&ui_scrBLE, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_scrBLE_screen_init);
}

void ScreenBLE::update(BLE &ble) {

  if(ble.isConnected() && !connected) {
    connected = true;
    lv_label_set_text(ui_lblState, "CONNECTED");
    ui_object_set_themeable_style_property(ui_lblState, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_statusConnected);
    ui_object_set_themeable_style_property(ui_statusRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusConnected);
    ui_object_set_themeable_style_property(ui_statusInsideRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusConnected);
    ui_object_set_themeable_style_property(lv_obj_get_child(ui_topBar, 0), LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_BG_COLOR, _ui_theme_color_statusConnected);
    ui_object_set_themeable_style_property(lv_obj_get_child(ui_topBar, 0), LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_BORDER_COLOR, _ui_theme_color_statusConnected);
    ui_object_set_themeable_style_property(lv_obj_get_child(ui_topBar, 3), LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_IMAGE_RECOLOR, _ui_theme_color_statusConnected);
  } else if(!ble.isConnected() && connected) {
    connected = false;
    lv_label_set_text(ui_lblState, "ADVERTISING");
    ui_object_set_themeable_style_property(ui_lblState, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_statusAdvertisin);
    ui_object_set_themeable_style_property(ui_statusRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusAdvertisin);
    ui_object_set_themeable_style_property(ui_statusInsideRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusAdvertisin);
    ui_object_set_themeable_style_property(lv_obj_get_child(ui_topBar, 0), LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_BG_COLOR, _ui_theme_color_statusAdvertisin);
    ui_object_set_themeable_style_property(lv_obj_get_child(ui_topBar, 0), LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_BORDER_COLOR, _ui_theme_color_statusAdvertisin);
    ui_object_set_themeable_style_property(lv_obj_get_child(ui_topBar, 3), LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_IMAGE_RECOLOR, _ui_theme_color_statusAdvertisin);
  }
}

void ScreenBLE::hide() {

}

ScreenBLE::~ScreenBLE() {
  ui_scrBLE_screen_destroy();
}