#include "../ui/ui.h"
#include "screens.hpp"
#include "topbar_status.hpp"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(screen_ble, LOG_LEVEL_INF);

ScreenBLE::ScreenBLE(BLE &ble, AHT10Service &ahtService, MPU6050Service &accelService)
  : ble(ble), ahtService(ahtService), accelService(accelService) { }

ScreenBLE::~ScreenBLE() { }

void ScreenBLE::show()
{
  _ui_screen_change(&ui_scrBLE, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_scrBLE_screen_init);
  connected = !ble.isConnected();
  // Freshly-created widgets already default to "unsubscribed"/off, so the
  // caches must start there too. If we instead started from the opposite of
  // the real state (as connected does above, harmlessly), update() would see
  // a "just unsubscribed" transition on the very first tick whenever a
  // characteristic is NOT subscribed - the common case - and decrement
  // notifyCount below zero, wrapping the uint8_t to 255.
  temperatureSubscribed = false;
  humiditySubscribed = false;
  accelSubscribed = false;
  notifyCount = 0;
  displayedNotifyCount = 0;
}

void ScreenBLE::hide()
{
  if(ui_scrBLE != NULL) {
    ui_scrBLE_screen_destroy();
  }
}

void ScreenBLE::update()
{
  updateConnectionStatus(ble);
  updateNotifyTemperature(ahtService);
  updateNotifyHumidity(ahtService);
  updateNotifyAccel(accelService);
  // Update label with characteristic notification count
  updateNotifyCount();
  updateRSSI();
}

bool ScreenBLE::isActive()
{
  return lv_screen_active() == ui_scrBLE;
}

void ScreenBLE::updateConnectionStatus(BLE &ble)
{
  if(ble.isConnected() && !connected) {
    connected = true;
    lv_label_set_text(ui_lblState, "CONNECTED");
    ui_object_set_themeable_style_property(ui_lblState, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_statusConnected);
    ui_object_set_themeable_style_property(ui_statusRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusConnected);
    ui_object_set_themeable_style_property(ui_statusInsideRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusConnected);
    setTopbarConnectionStatus(ui_topBar, true);
  } else if(!ble.isConnected() && connected) {
    connected = false;
    lv_label_set_text(ui_lblState, "ADVERTISING");
    ui_object_set_themeable_style_property(ui_lblState, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, _ui_theme_color_statusAdvertisin);
    ui_object_set_themeable_style_property(ui_statusRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusAdvertisin);
    ui_object_set_themeable_style_property(ui_statusInsideRing, LV_PART_INDICATOR| LV_STATE_DEFAULT, LV_STYLE_ARC_COLOR, _ui_theme_color_statusAdvertisin);
    setTopbarConnectionStatus(ui_topBar, false);
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

void ScreenBLE::updateNotifyAccel(MPU6050Service &service)
{
  // A single BLE characteristic now carries all 3 axes together, so all
  // three switches simply mirror that one subscription as a group.
  bool subscribed = service.accelIsSubscribed();
  if(subscribed && !accelSubscribed) {
    accelSubscribed = true;
    notifyCount += 3;
    setAccelSwitches(true);
  } else if(!subscribed && accelSubscribed) {
    accelSubscribed = false;
    notifyCount -= 3;
    setAccelSwitches(false);
  }
}

void ScreenBLE::setAccelSwitches(bool on)
{
  const char *text = on ? "ON" : "-";
  const ui_theme_variable_t *color = on ? _ui_theme_color_statusConnected : _ui_theme_color_textSecondary;

  lv_label_set_text(ui_notifyItem3SwitchLbl, text);
  ui_object_set_themeable_style_property(ui_notifyItem3SwitchLbl, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, color);
  lv_obj_set_state(ui_notifyItem3Switch, LV_STATE_CHECKED, on);

  lv_label_set_text(ui_notifyItem4SwitchLbl, text);
  ui_object_set_themeable_style_property(ui_notifyItem4SwitchLbl, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, color);
  lv_obj_set_state(ui_notifyItem4Switch, LV_STATE_CHECKED, on);

  lv_label_set_text(ui_notifyItem5SwitchLbl, text);
  ui_object_set_themeable_style_property(ui_notifyItem5SwitchLbl, LV_PART_MAIN| LV_STATE_DEFAULT, LV_STYLE_TEXT_COLOR, color);
  lv_obj_set_state(ui_notifyItem5Switch, LV_STATE_CHECKED, on);
}

void ScreenBLE::updateNotifyCount(void)
{
  if(displayedNotifyCount != notifyCount) {
    lv_label_set_text_fmt(ui_notifyCount, "%d/5", notifyCount);
    displayedNotifyCount = notifyCount;
  }
}

void ScreenBLE::updateRSSI(void) {
  int16_t rssi_av = 0;
  for(uint8_t i = 0; i < 10; i++) {
    int8_t rssi;
    ble.readRSSI(&rssi);
    rssi_av += rssi;
  }
  rssi_av /= 10;
  if(connected) {
    lv_label_set_text_fmt(ui_lblSubHero, "RSSI: %d dBm", rssi_av);
  } else {
    lv_label_set_text_fmt(ui_lblSubHero, "RSSI: --");
  }
}