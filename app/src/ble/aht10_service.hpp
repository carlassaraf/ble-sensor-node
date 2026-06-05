#pragma once

#include "service.hpp"
#include "characteristic.hpp"

class AHT10Service : Service {
public:
  AHT10Service();

  static void onTempCccChanged(const struct bt_gatt_attr *attr, uint16_t value);
  static void onHumCccChanged(const struct bt_gatt_attr *attr, uint16_t value);

  /** @brief Returns client's notification request to temperature characteristic */
  bool tempIsSubscribed() { return tempSubscription; }
  /** @brief Returns client's notification request to humidity characteristic */
  bool humIsSubscribed() { return humSubscription; }

private:
  static inline AHT10Service *instance = nullptr;
  Characteristic<int16_t> temp;
  Characteristic<uint16_t> hum;
  bool tempSubscription{false};
  bool humSubscription{false};
};