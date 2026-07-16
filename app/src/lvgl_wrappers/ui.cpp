#include "ui/ui.h"
#include "screens/screens.hpp"
#include "ui.hpp"

UI::UI(BLE &ble, AHT10Service &ahtService, MPU6050Service &accelService, AHT10 &aht, MPU6050 &mpu, const struct gpio_dt_spec &btn)
  : scrBle(ble, ahtService, accelService), scrClimate(aht, ble), scrMotion(mpu, ble), btn(btn)
{
  lvgl.start();

  lvgl.lock();
  LV_EVENT_GET_COMP_CHILD = lv_event_register_id();
  lv_disp_t *dispp = lv_display_get_default();
  lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
  lv_display_set_theme(dispp, theme);
  lvgl.unlock();

  goTo(Screens::BLE);
}

UI::~UI()
{

}

void UI::run()
{
  while (1) {
    lvgl.lock();
    scrActive->update();
    lvgl.unlock();
    pollButton();
    k_msleep(100);
  }
}

void UI::pollButton()
{
  bool pressed = gpio_pin_get_dt(&btn) == 1;
  if (pressed && !btnPressed) {
    int next = (static_cast<int>(activeScreen()) + 1) % static_cast<int>(Screens::ScreenCount);
    goTo(static_cast<Screens>(next));
  }
  btnPressed = pressed;
}

Screen &UI::screenFor(Screens screen)
{
  switch (screen) {
    case Screens::BLE:      return scrBle;
    case Screens::Climate:  return scrClimate;
    case Screens::Motion:   return scrMotion;
  }
  return scrBle;
}

Screens UI::activeScreen()
{
  if (scrActive == &scrBle) {
    return Screens::BLE;
  } else if (scrActive == &scrClimate) {
    return Screens::Climate;
  } else if (scrActive == &scrMotion) {
    return Screens::Motion;
  }
  return Screens::BLE;
}

void UI::goTo(Screens screen)
{
  lvgl.lock();
  scrActive->hide();
  scrActive = &screenFor(screen);
  scrActive->show();
  lvgl.unlock();
}