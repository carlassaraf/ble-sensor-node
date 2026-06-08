#pragma once

#include "service.hpp"
#include "characteristic.hpp"
#include "aht10/aht10.hpp"
#include "zephyr_wrappers/thread.hpp"
#include "led/led.hpp"

class AHT10Service;

class AHT10ServiceThread : public Thread {
public:
  AHT10ServiceThread(k_thread_stack_t *stack, size_t stack_size, int prio, AHT10 &aht10) 
  : Thread(stack, stack_size, prio), aht10(aht10) { }

protected:
  void run(void *context) override;

private:
  AHT10 &aht10;
};

class AHT10Service : Service {
public:
  AHT10Service(AHT10 &aht10, LED led);

  static void onTempCccChanged(const struct bt_gatt_attr *attr, uint16_t value);
  static void onHumCccChanged(const struct bt_gatt_attr *attr, uint16_t value);

  /** @brief Returns client's notification request to temperature characteristic */
  bool tempIsSubscribed() { return tempSubscription; }
  /** @brief Returns client's notification request to humidity characteristic */
  bool humIsSubscribed() { return humSubscription; }
  /** @brief Starts BLE service thread */
  void start() { thread.start(this); }

  LED led;

private:
  static inline AHT10Service *instance = nullptr;
  Characteristic<int16_t> temp;
  Characteristic<uint16_t> hum;
  bool tempSubscription{false};
  bool humSubscription{false};
  AHT10ServiceThread thread;
};