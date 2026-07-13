#include "ui/ui.h"
#include "screens/screens.hpp"
#include "ui.hpp"

UI::UI(BLE &ble, AHT10Service &ahtService, AHT10 &aht)
  : scrBle(ble, ahtService), scrClimate(aht)
{
  lvgl.start();
  LV_EVENT_GET_COMP_CHILD = lv_event_register_id();
  lv_disp_t *dispp = lv_display_get_default();
  lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
  lv_display_set_theme(dispp, theme);
  scrClimate.show();
  // ui_scrBLE_screen_init();
  // ui_scrClimate_screen_init();
  // ui____initial_actions0 = lv_obj_create(NULL);
  // lv_disp_load_scr( ui_scrBLE);
}

UI::~UI()
{

}

void UI::run()
{
  while (1) {
    lvgl.lock();
    for (Screen *s : screens) {
      if (s->isActive()) {
        s->update();
        break;
      }
    }
    lvgl.unlock();
    k_msleep(100);
  }
}