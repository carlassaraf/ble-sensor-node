#pragma once

#include "zephyr_wrappers/thread.hpp"

class LVGL : Thread {

public:
  LVGL();

  void lock();
  void unlock();
  void run(void *context);
  void start();

private:
  LVGL *instance;
};