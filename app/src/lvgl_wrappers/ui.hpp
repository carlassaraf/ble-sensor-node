#pragma once

#include "lvgl_port.hpp"
#include "screens/screens.hpp"

class UI {

public:
  UI(BLE &ble, AHT10Service &ahtService, AHT10 &aht);
  ~UI();
  void run();

private:
  LVGL lvgl;
  ScreenBLE scrBle;
  ScreenClimate scrClimate;
  Screen *screens[2] = {&scrBle, &scrClimate};
};