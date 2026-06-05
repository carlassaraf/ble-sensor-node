#include "ble.hpp"

LOG_MODULE_REGISTER(ble, LOG_LEVEL_INF);

namespace ble_params {

  const uint16_t company_id = 0x0059; /* Nordic Semiconductor ASA */
  const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(BT_LE_ADV_OPT_NONE, /* No options specified */
    800, /* Min Advertising Interval 500ms (800*0.625ms) */
    801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
    NULL); /* Set to NULL for undirected advertising */
  const struct bt_data ad_packet[] = {
    /** Flags */
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    /** Complete device name and manufacturer ID */
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, (const uint8_t *)&company_id, sizeof(company_id)),
  };  
};

BLE::BLE() {
  instance = this;
  int err = bt_enable(BLE::onEnabled);
  if (err) {
    LOG_ERR("Bluetooth init failed (err %d)", err);
  }
}

bool BLE::startAdvertising() {
  int err = bt_le_adv_start(ble_params::adv_param, ble_params::ad_packet, ARRAY_SIZE(ble_params::ad_packet), NULL, 0);
  if (err) {
    LOG_ERR("Failed to start advertising (err %d)", err);
    return false;
  }
  LOG_INF("Advertising started successfully");
  return true;
}

void BLE::onEnabled(int err) {
  if (err) {
    LOG_ERR("Bluetooth init failed (err %d)", err);
    return;
  }
  instance->enabled = true;
  LOG_INF("Bluetooth initialized successfully");
}