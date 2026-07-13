#include "ui/ui.h"
#include "screens.hpp"

void ScreenBLE::show()
{
  _ui_screen_change(&ui_scrBLE, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_scrBLE_screen_init);
}

void ScreenBLE::update(BLE &ble, AHT10Service &service)
{
  updateConnectionStatus(ble);
  updateNotifyTemperature(service);
  updateNotifyHumidity(service);
  // Update label with characteristic notification count
  updateNotifyCount();
}

void ScreenBLE::hide()
{

}

bool ScreenBLE::isActive()
{
  return lv_screen_active() == ui_scrBLE;
}

ScreenBLE::~ScreenBLE()
{
  ui_scrBLE_screen_destroy();
}

void ScreenBLE::updateConnectionStatus(BLE &ble)
{
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

void ScreenBLE::updateNotifyTemperature(AHT10Service &service)
{
  if(service.tempIsSubscribed() && !temperatureSubscribed) {
    temperatureSubscribed = true;
    notifyCount++;
    lv_label_set_text(ui_notifyItem1SwitchLbl, "ON");
    ui_object_set_themeable_style_property(ui_notifyItem1SwitchLbl, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_statusConnected);
    lv_obj_set_state(ui_notifyItem1Switch, LV_STATE_CHECKED, true);
  } else if(!service.tempIsSubscribed() && temperatureSubscribed) {
    temperatureSubscribed = false;
    notifyCount--;
    lv_label_set_text(ui_notifyItem1SwitchLbl, "-");
    ui_object_set_themeable_style_property(ui_notifyItem1SwitchLbl, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_textSecondary);    
    lv_obj_set_state(ui_notifyItem1Switch, LV_STATE_CHECKED, false);
  }
}

void ScreenBLE::updateNotifyHumidity(AHT10Service &service)
{
  if(service.humIsSubscribed() && !humiditySubscribed) {
    humiditySubscribed = true;
    notifyCount++;
    lv_label_set_text(ui_notifyItem2SwitchLbl, "ON");
    ui_object_set_themeable_style_property(ui_notifyItem2SwitchLbl, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_statusConnected);
    lv_obj_set_state(ui_notifyItem2Switch, LV_STATE_CHECKED, true);
  } else if(!service.humIsSubscribed() && humiditySubscribed) {
    humiditySubscribed = false;
    notifyCount--;
    lv_label_set_text(ui_notifyItem2SwitchLbl, "-");
    ui_object_set_themeable_style_property(ui_notifyItem2SwitchLbl, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_textSecondary);    
    lv_obj_set_state(ui_notifyItem2Switch, LV_STATE_CHECKED, false);
  }
}

void ScreenBLE::updateNotifyCount(void)
{
  if(displayedNotifyCount != notifyCount) {
    lv_label_set_text_fmt(ui_notifyCount, "%d/5", notifyCount);
    displayedNotifyCount = notifyCount;
  }
}