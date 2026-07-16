#pragma once

#include <zephyr/drivers/gpio.h>
#include "lvgl_port.hpp"
#include "screens/screens.hpp"

enum class Screens {
  BLE,
  Climate,
  Motion,
  ScreenCount
};

class UI {

public:
  UI(BLE &ble, AHT10Service &ahtService, AHT10 &aht, MPU6050 &mpu, const struct gpio_dt_spec &btn);
  ~UI();
  void run();
  void goTo(Screens screen);

private:
  LVGL lvgl;
  ScreenBLE scrBle;
  ScreenClimate scrClimate;
  ScreenMotion scrMotion;
  Screen *scrActive = &scrBle;
  struct gpio_dt_spec btn;
  bool btnPressed{false};

  Screen &screenFor(Screens screen);
  Screens activeScreen();
  void pollButton();
};