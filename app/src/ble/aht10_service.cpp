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
#include "zephyr_wrappers/thread.hpp"

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

namespace ess_thread {
  constexpr uint32_t stack_size = 1024;
  constexpr uint32_t prio = 5;
  K_THREAD_STACK_DEFINE(thread_stask, stack_size);
};

AHT10Service::AHT10Service(AHT10 &aht10) : 
  thread(AHT10ServiceThread(ess_thread::thread_stask, ess_thread::stack_size, ess_thread::prio, aht10))
{
  instance = this;
}

void AHT10ServiceThread::run(void *context)
{
  auto *service = static_cast<AHT10Service *>(context);
  while (1) {
    if (service->humIsSubscribed() && aht10.isInitialized()) {
      float hum = 0.0;
      aht10.readHumidity(hum);
      uint16_t hum_val = static_cast<uint16_t>(hum * 100);
      bt_gatt_notify(NULL, &ess::svc.attrs[5], &hum_val, sizeof(hum_val));
    }
    if (service->tempIsSubscribed() && aht10.isInitialized()) {
      float temp = 0.0;
      aht10.readTemperature(temp);
      int16_t temp_val = static_cast<int16_t>(temp * 100);
      bt_gatt_notify(NULL, &ess::svc.attrs[2], &temp_val, sizeof(temp_val));
    }
    sleep_ms(100);
  }
}

void AHT10Service::onHumCccChanged(const struct bt_gatt_attr *attr, uint16_t value)
{
  instance->humSubscription = (value == BT_GATT_CCC_NOTIFY);
  LOG_DBG("Client has %s humidity", instance->humSubscription ? "subscribed" : "unsubscribed");
}

void AHT10Service::onTempCccChanged(const struct bt_gatt_attr *attr, uint16_t value)
{
  instance->tempSubscription = (value == BT_GATT_CCC_NOTIFY);
  LOG_DBG("Client has %s temperature", instance->tempSubscription ? "subscribed" : "unsubscribed");
}