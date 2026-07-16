#include "../ui/ui.h"
#include "screens.hpp"
#include "topbar_status.hpp"

ScreenMotion::ScreenMotion(MPU6050 &mpu, BLE &ble) : mpu(mpu), ble(ble) { }

ScreenMotion::~ScreenMotion()
{
  if(ui_scrMotion != NULL) {
    ui_scrMotion_screen_destroy();
  } 
}

void ScreenMotion::show()
{
  _ui_screen_change(&ui_scrMotion, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_scrMotion_screen_init);
  connected = !ble.isConnected();
}

void ScreenMotion::hide()
{
  if(ui_scrMotion != NULL) {
    ui_scrMotion_screen_destroy();
  }
}

void ScreenMotion::update()
{
  updateConnectionStatus();
  if(mpu.isInitialized()) {

  }
}

void ScreenMotion::updateConnectionStatus()
{
  if(ble.isConnected() && !connected) {
    connected = true;
    setTopbarConnectionStatus(ui_topBar2, true);
  } else if(!ble.isConnected() && connected) {
    connected = false;
    setTopbarConnectionStatus(ui_topBar2, false);
  }
}

bool ScreenMotion::isActive()
{
  return lv_screen_active() == ui_scrMotion;
}