#include "ble.hpp"

LOG_MODULE_REGISTER(ble, LOG_LEVEL_INF);

namespace ble_params {

  const uint16_t company_id = 0x0059; /* Nordic Semiconductor ASA */
  const uint16_t service_uuid = 0x181A; /* Environmental Sensing Service */
  const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM((BT_LE_ADV_OPT_CONN|BT_LE_ADV_OPT_USE_IDENTITY), /* Connection is possible */
    800, /* Min Advertising Interval 500ms (800*0.625ms) */
    801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
    NULL); /* Set to NULL for undirected advertising */
  const struct bt_data ad_packet[] = {
    /** Flags */
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    /** Complete device name, manufacturer ID, service UUID */
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, (const uint8_t *)&company_id, sizeof(company_id)),
    BT_DATA(BT_DATA_UUID16_SOME, (const uint8_t *)&service_uuid, sizeof(service_uuid)),
  };
  // Connection callbacks
  static struct bt_conn_cb conn_callbacks = {};
  static struct bt_conn_auth_cb conn_auth_callbacks = {};
};

BLE::BLE(LED advLED, LED connLED, LED disLED) : advLED(advLED), connLED(connLED), disLED(disLED) {
  instance = this;
  // Register connection callbacks
  ble_params::conn_callbacks.connected = BLE::onConnected;
  ble_params::conn_callbacks.disconnected = BLE::onDisconnected;
  ble_params::conn_callbacks.recycled = BLE::onRecycled;
  bt_conn_cb_register(&ble_params::conn_callbacks);
  // Register authorization callbacks
  ble_params::conn_auth_callbacks.cancel = BLE::auth_cancel;
  ble_params::conn_auth_callbacks.passkey_display = BLE::auth_passkey_display;
  bt_conn_auth_cb_register(&ble_params::conn_auth_callbacks);
  // Initiate work item for advertising
  k_work_init(&adv_work, BLE::adv_work_handler);
  // Enable Bluetooth
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
  LOG_DBG("Advertising started successfully");
  advLED.on();
  return true;
}

void BLE::adv_work_handler(struct k_work *work) {
  instance->startAdvertising();
}

void BLE::onEnabled(int err) {
  if (err) {
    LOG_ERR("Bluetooth init failed (err %d)", err);
    return;
  }
  instance->enabled = true;
  LOG_DBG("Bluetooth initialized successfully");
}

void BLE::onConnected(struct bt_conn *conn, uint8_t err) {
  if (err) {
    LOG_ERR("Connection failed (err %d)", err);
    return;
  }
  // Increment reference count
  instance->current_conn = bt_conn_ref(conn);
  instance->connected = true;
  instance->advLED.off();
  instance->disLED.off();
  instance->connLED.on();
  LOG_DBG("Connected");
}

void BLE::onDisconnected(struct bt_conn *conn, uint8_t reason) {
  // Decrement reference count
  bt_conn_unref(conn);
  instance->connected = false;
  instance->connLED.off();
  instance->disLED.on();
  LOG_DBG("Disconnected (reason %d)", reason);
}

void BLE::onRecycled() {
  LOG_DBG("Connection object recycled, can start new connection or advertiser");
  // Handle advertising outside BT stack context
  k_work_submit(&instance->adv_work);
}

void BLE::auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
  char addr[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
  LOG_INF("Passkey for %s: %06u\n", addr, passkey);
}

void BLE::auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	LOG_INF("Pairing cancelled: %s\n", addr);
}