#ifndef _AHT10_H_
#define _AHT10_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>

// Device command definitions for AHT10 sensor
#define AHT10_CMD_INIT      0xE1
#define AHT10_CMD_TRIGGER   0xAC

// Device API structure for AHT10 sensor

__subsystem struct aht10_driver_api {
  /**
   * @brief Read the temperature from the AHT10 sensor.
   *
   * @param dev AHT10 device instance.
   * @param temperature Pointer to store the read temperature.
   *
   * @retval 0 if successful.
   * @retval -errno Other negative errno code on failure.
   */
  int (*read_temperature)(const struct device *dev, float *temperature);
  /**
   * @brief Read the humidity from the AHT10 sensor.
   *
   * @param dev AHT10 device instance.
   * @param humidity Pointer to store the read humidity.
   *
   * @retval 0 if successful.
   * @retval -errno Other negative errno code on failure.
   */
  int (*read_humidity)(const struct device *dev, float *humidity);
};

__syscall int aht10_read_temperature(const struct device *dev, float *temperature);

static inline int z_impl_aht10_read_temperature(const struct device *dev, float *temperature)
{
  __ASSERT_NO_MSG(DEVICE_API_IS(aht10, dev));  
  return DEVICE_API_GET(aht10, dev)->read_temperature(dev, temperature);
}

__syscall int aht10_read_humidity(const struct device *dev, float *humidity);

static inline int z_impl_aht10_read_humidity(const struct device *dev, float *humidity)
{
  __ASSERT_NO_MSG(DEVICE_API_IS(aht10, dev));  
  return DEVICE_API_GET(aht10, dev)->read_humidity(dev, humidity);
}

#include <syscalls/aht10.h>

#endif