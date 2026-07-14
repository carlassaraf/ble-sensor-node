#pragma once

#include <zephyr/drivers/gpio.h>
#include "lvgl_port.hpp"
#include "screens/screens.hpp"

enum class Screens {
  BLE,
  Climate
};

class UI {

public:
  UI(BLE &ble, AHT10Service &ahtService, AHT10 &aht, const struct gpio_dt_spec &btn);
  ~UI();
  void run();
  void goTo(Screens screen);

private:
  LVGL lvgl;
  ScreenBLE scrBle;
  ScreenClimate scrClimate;
  Screen *scrActive = &scrBle;
  struct gpio_dt_spec btn;
  bool btnPressed{false};

  Screen &screenFor(Screens screen);
  void pollButton();
};