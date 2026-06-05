#pragma once

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>

class BLE {
public:
  BLE();
  bool startAdvertising();
  bool isEnabled() const { return enabled; }

private:
  static inline BLE *instance = nullptr;
  bool enabled{false};
  static void onEnabled(int err);
};