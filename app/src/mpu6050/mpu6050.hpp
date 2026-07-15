#pragma once

#include <zephyr/drivers/sensor.h>

class MPU6050 {

public:
  explicit MPU6050(const struct device *const mpu6050) : mpu6050_dev(mpu6050), initialized(device_is_ready(mpu6050)) { }
  ~MPU6050() = default;
  bool isInitialized() const { return initialized; }
  bool readAccel(double *accel);
  bool readGyro(double *gyro);
  bool readTemp(double *temp);

private:
  const struct device *const mpu6050_dev;
  bool initialized;
};