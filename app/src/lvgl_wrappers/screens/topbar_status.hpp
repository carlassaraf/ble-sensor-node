#pragma once

#include "../ui/ui.h"

inline void setTopbarConnectionStatus(lv_obj_t *topbar, bool connected)
{
  const ui_theme_variable_t *color = connected ? _ui_theme_color_statusConnected : _ui_theme_color_statusAdvertisin;
  ui_object_set_themeable_style_property(lv_obj_get_child(topbar, 0), LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_COLOR, color);
  ui_object_set_themeable_style_property(lv_obj_get_child(topbar, 0), LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BORDER_COLOR, color);
  ui_object_set_themeable_style_property(lv_obj_get_child(topbar, 3), LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_IMAGE_RECOLOR, color);
}
