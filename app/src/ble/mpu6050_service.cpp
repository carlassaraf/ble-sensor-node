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

#include "mpu6050_service.hpp"

LOG_MODULE_REGISTER(mpu6050_service, LOG_LEVEL_INF);

// Custom Acceleration Service, exposing a single SIG "Acceleration"
// characteristic (0x2C06) whose value carries all 3 axes packed together
// (sint32 each, m/s^2 with 1mm/s^2 resolution). Earlier this used one
// characteristic instance per axis, all sharing the same 0x2C06 UUID; BLE
// client stacks (notably iOS CoreBluetooth) track/route notifications by
// UUID and silently dropped/misattributed them when a service had multiple
// characteristics with an identical UUID. A single characteristic removes
// that ambiguity entirely.
namespace accel {
  struct __packed accel_data {
    int32_t x;
    int32_t y;
    int32_t z;
  };

  constexpr uint16_t char_uuid_val = 0x2C06;
  #define ACCEL_CHAR_UUID BT_UUID_DECLARE_16(accel::char_uuid_val)

  BT_GATT_SERVICE_DEFINE(svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_128(
      BT_UUID_128_ENCODE(0xdb54c008, 0x459e, 0x497b, 0x8e20, 0xfa1d93a3f897))),
    BT_GATT_CHARACTERISTIC(ACCEL_CHAR_UUID, BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CUD("Acceleration", BT_GATT_PERM_READ),
    BT_GATT_CCC(MPU6050Service::onAccelCccChanged, BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN),
  );

  #undef ACCEL_CHAR_UUID

  // Value attribute index within svc.attrs[]: primary service (0),
  // declaration (1), value (2), CUD (3), CCC (4).
  constexpr size_t val_idx = 2;
};

namespace accel_thread {
  constexpr uint32_t stack_size = 1024;
  constexpr uint32_t prio = 5;
  K_THREAD_STACK_DEFINE(thread_stack, stack_size);
};

MPU6050Service::MPU6050Service(MPU6050 &mpu) :
  thread(MPU6050ServiceThread(accel_thread::thread_stack, accel_thread::stack_size, accel_thread::prio, mpu))
{
  instance = this;
}

void MPU6050ServiceThread::run(void *context)
{
  auto *service = static_cast<MPU6050Service *>(context);
  bool wasActive = false;

  while (1) {
    bool active = service->accelIsSubscribed();
    if (active != wasActive) {
      wasActive = active;
      LOG_INF("Acceleration notifications %s", active ? "active" : "idle");
    }

    if (active) {
      if (!mpu.isInitialized()) {
        LOG_WRN("Subscribed but MPU6050 is not initialized, skipping notify");
      } else {
        double accel[3];
        if (!mpu.readAccel(accel)) {
          LOG_ERR("Failed to read acceleration data");
        } else {
          // Base Unit: m/s^2, resolution 0.001 m/s^2 (1mm/s^2)
          accel::accel_data data = {
            .x = static_cast<int32_t>(accel[0] * 1000.0),
            .y = static_cast<int32_t>(accel[1] * 1000.0),
            .z = static_cast<int32_t>(accel[2] * 1000.0),
          };
          int err = bt_gatt_notify(NULL, &accel::svc.attrs[accel::val_idx], &data, sizeof(data));
          if (err) {
            LOG_ERR("Failed to notify acceleration (err %d)", err);
          } else {
            LOG_DBG("Notified acceleration x=%d y=%d z=%d", data.x, data.y, data.z);
          }
        }
      }
    }
    sleep_ms(100);
  }
}

void MPU6050Service::onAccelCccChanged(const struct bt_gatt_attr *attr, uint16_t value)
{
  instance->accelSubscription = (value == BT_GATT_CCC_NOTIFY);
  LOG_INF("Client has %s acceleration", instance->accelSubscription ? "subscribed" : "unsubscribed");
}
