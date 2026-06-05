#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "aht10_service.hpp"

LOG_MODULE_REGISTER(aht10_service, LOG_LEVEL_INF);

// Environmental Sensing Service
namespace ess {
  BT_GATT_SERVICE_DEFINE(svc,
      BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),
      BT_GATT_CHARACTERISTIC(BT_UUID_TEMPERATURE, BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
      BT_GATT_CCC(AHT10Service::onTempCccChanged, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
      BT_GATT_CHARACTERISTIC(BT_UUID_HUMIDITY,    BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
      BT_GATT_CCC(AHT10Service::onHumCccChanged,  BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
  );
};

AHT10Service::AHT10Service() 
{
  instance = this;
}

void AHT10Service::onHumCccChanged(const struct bt_gatt_attr *attr, uint16_t value)
{
  instance->tempSubscription = (value == BT_GATT_CCC_NOTIFY);
  LOG_DBG("Client has %s", instance->tempSubscription? "subscribed" : "unsubscribed");
}

void AHT10Service::onTempCccChanged(const struct bt_gatt_attr *attr, uint16_t value)
{
  instance->humSubscription = (value == BT_GATT_CCC_NOTIFY);
  LOG_DBG("Client has %s", instance->humSubscription? "subscribed" : "unsubscribed");
}