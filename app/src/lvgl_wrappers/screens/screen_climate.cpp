#include "../ui/ui.h"
#include "screens.hpp"

ScreenClimate::ScreenClimate(AHT10 &aht) : aht(aht) { }

ScreenClimate::~ScreenClimate()
{
  if(ui_scrClimate != NULL) {
    ui_scrClimate_screen_destroy();
  } 
}

void ScreenClimate::show()
{
  _ui_screen_change(&ui_scrClimate, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_scrClimate_screen_init);
}

void ScreenClimate::hide()
{
  if(ui_scrClimate != NULL) {
    ui_scrClimate_screen_destroy();
  }
}

void ScreenClimate::update()
{
  if(aht.isInitialized()) {
    float temp = 0.0, hum = 0.0;
    aht.readTemperature(temp);
    lv_arc_set_value(ui_tempArc, (int32_t)temp);
    lv_label_set_text_fmt(ui_tempValue, "%4.1f", (double)temp);
    aht.readHumidity(hum);
    lv_arc_set_value(ui_humArc, (int32_t)hum);
    lv_label_set_text_fmt(ui_humValue, "%4.1f", (double)hum);
  }
}

bool ScreenClimate::isActive()
{
  return lv_screen_active() == ui_scrClimate;
}