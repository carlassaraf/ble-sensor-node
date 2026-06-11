#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include "lvgl_port.hpp"

LOG_MODULE_REGISTER(lvgl_port, LOG_LEVEL_INF);

namespace lvgl_thread {

  static const uint32_t stack_size = 8192;
  static const uint32_t priority = 5;

  K_THREAD_STACK_DEFINE(stack, stack_size);
  K_MUTEX_DEFINE(mutex);
}

LVGL::LVGL() : Thread(lvgl_thread::stack, lvgl_thread::stack_size, lvgl_thread::priority)
{
  instance = this;
}

void LVGL::lock()
{
  k_mutex_lock(&lvgl_thread::mutex, K_FOREVER);
}

void LVGL::unlock()
{
  k_mutex_unlock(&lvgl_thread::mutex);
}

void LVGL::run(void *context)
{
  auto *lvgl = static_cast<LVGL*>(context);
  while(1) {
    lvgl->lock();
    lv_task_handler();
    lvgl->unlock();
    lvgl->sleep_ms(10);
  }
}

void LVGL::start() {
  /* Set up display and initial UI before starting the LVGL thread. */
  const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
  if (!device_is_ready(display)) {
    LOG_ERR("Display device not ready");
    return;
  }
  int ret = display_blanking_off(display);
  LOG_INF("blanking_off: %d", ret);

  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0xff00ff), LV_PART_MAIN);
  lv_obj_t *lbl_temp = lv_label_create(scr);
  lv_obj_t *lbl_hum  = lv_label_create(scr);
  lv_obj_set_pos(lbl_temp, 10, 10);
  lv_obj_set_pos(lbl_hum,  10, 80);
  lv_label_set_text(lbl_temp, "Temp: --");
  lv_label_set_text(lbl_hum,  "Hum:  --");

  Thread::start(this);
}