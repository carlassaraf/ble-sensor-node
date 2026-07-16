#include "../ui/ui.h"
#include "screens.hpp"
#include "topbar_status.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(screen_motion, LOG_LEVEL_INF);

#include <math.h>

namespace {

constexpr double RAD_TO_DEG = 180.0 / 3.14159265358979323846;

// LVGL image rotation is expressed in 0.1 degree units, clockwise, 0..3600.
int32_t accelToRotation(double ax, double ay)
{
  double angle = atan2(-ax, ay) * RAD_TO_DEG;
  if(angle < 0.0) {
    angle += 360.0;
  }
  return static_cast<int32_t>(angle * 10.0);
}

} // namespace

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
  // ui_lvl is a half-disc image top-aligned in ui_ring; its flat edge sits at
  // the ring's center, so the rotation pivot must be its bottom-mid point
  // rather than LVGL's default image-center pivot.
  lv_image_set_pivot(ui_lvl, LV_PCT(50), LV_PCT(100));
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
    double accel[3];
    if(!mpu.readAccel(accel)) {
      return;
    }

    lv_label_set_text_fmt(ui_axisXv, "%.2f", accel[0]);
    lv_label_set_text_fmt(ui_axisYv, "%.2f", accel[1]);
    lv_label_set_text_fmt(ui_axisZv, "%.2f", accel[2]);

    lv_image_set_rotation(ui_lvl, accelToRotation(accel[0], accel[1]));
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