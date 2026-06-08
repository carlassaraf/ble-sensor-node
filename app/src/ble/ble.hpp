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
  bool isConnected() const { return connected; }

private:
  static inline BLE *instance = nullptr;
  static inline bt_conn *current_conn = nullptr;
  bool enabled{false};
  bool connected{false};
  struct k_work adv_work;
  static void adv_work_handler(struct k_work *work);
  static void onEnabled(int err);
  static void onConnected(struct bt_conn *conn, uint8_t err);
  static void onDisconnected(struct bt_conn *conn, uint8_t reason);
  static void onRecycled();

  static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey);
  static void auth_cancel(struct bt_conn *conn);
};