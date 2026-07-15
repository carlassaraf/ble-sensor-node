#include "mpu6050.hpp"

bool MPU6050::readAccel(double *accel)
{
  int ret = sensor_sample_fetch(mpu6050_dev);
  if(ret) {
    return false;
  }
  struct sensor_value foo[3];
  ret = sensor_channel_get(mpu6050_dev, SENSOR_CHAN_ACCEL_XYZ, foo);
  if(ret) {
    return false;
  }
  for(uint8_t i = 0; i < 3; i++) {
    *(accel++) = sensor_value_to_double(&foo[i]);
  }
  return true;
}

bool MPU6050::readGyro(double *gyro)
{
  int ret = sensor_sample_fetch(mpu6050_dev);
  if(ret) {
    return false;
  }
  struct sensor_value foo[3];
  ret = sensor_channel_get(mpu6050_dev, SENSOR_CHAN_GYRO_XYZ, foo);
  if(ret) {
    return false;
  }
  for(uint8_t i = 0; i < 3; i++) {
    gyro[i] = sensor_value_to_double(&foo[i]);
  }
  return true;
}

bool MPU6050::readTemp(double *temp)
{
  int ret = sensor_sample_fetch(mpu6050_dev);
  if(ret) {
    return false;
  }
  struct sensor_value temperature;
  ret = sensor_channel_get(mpu6050_dev, SENSOR_CHAN_DIE_TEMP, &temperature);
  if(ret) {
    return false;
  }
  *temp = sensor_value_to_double(&temperature);
  return true;
}