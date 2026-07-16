#pragma once

#include "service.hpp"
#include "mpu6050/mpu6050.hpp"
#include "zephyr_wrappers/thread.hpp"

class MPU6050Service;

class MPU6050ServiceThread : public Thread {
public:
  MPU6050ServiceThread(k_thread_stack_t *stack, size_t stack_size, int prio, MPU6050 &mpu6050)
  : Thread(stack, stack_size, prio), mpu(mpu6050) { }

protected:
  void run(void *context) override;

private:
  MPU6050 &mpu;
};

class MPU6050Service : Service {
public:
  MPU6050Service(MPU6050 &mpu);

  static void onAccelCccChanged(const struct bt_gatt_attr *attr, uint16_t value);

  /** @brief Returns client's notification request to the acceleration characteristic */
  bool accelIsSubscribed() { return accelSubscription; }
  /** @brief Starts BLE service thread */
  void start() { thread.start(this); }

private:
  static inline MPU6050Service *instance = nullptr;
  bool accelSubscription{false};
  MPU6050ServiceThread thread;
};
