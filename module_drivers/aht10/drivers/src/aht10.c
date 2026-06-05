#define DT_DRV_COMPAT asair_aht10

#include <zephyr/device.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "aht10.h"

LOG_MODULE_REGISTER(aht10, CONFIG_AHT10_LOG_LEVEL);

// AHT10 driver configuration structure
struct aht10_config {
  struct i2c_dt_spec i2c;
};

// AHT10 driver data structure
struct aht10_data {
  float temperature;
  float humidity;
};

// Internal implementations
static int read_raw(const struct device *dev)
{
  const struct aht10_config *config = dev->config;
  struct aht10_data *data = dev->data;  
  // Read raw data from the AHT10 sensor using I2C
  // This is a placeholder, actual I2C read commands should be implemented here
  LOG_DBG("Reading raw data from AHT10 sensor");
  uint8_t trigger[] = {AHT10_CMD_TRIGGER, 0x33, 0x00};
  if (i2c_write_dt(&config->i2c, trigger, sizeof(trigger)) < 0) {
    LOG_ERR("Failed to trigger AHT10 sensor");
    return -EIO;
  }
  k_msleep(80); // Wait for the sensor to process the trigger command
  uint8_t raw_data[6];
  if (i2c_read_dt(&config->i2c, raw_data, sizeof(raw_data)) < 0) {
    LOG_ERR("Failed to read data from AHT10 sensor");
    return -EIO;
  }
  // Process raw data to extract temperature and humidity values
  uint32_t raw_humidity = ((uint32_t)raw_data[1] << 12) | ((uint32_t)raw_data[2] << 4) | ((uint32_t)(raw_data[3] & 0xF0) >> 4);
  data->humidity = ((float)raw_humidity / (1 << 20)) * 100.0f;
  uint32_t raw_temperature = ((uint32_t)(raw_data[3] & 0x0F) << 16) | ((uint32_t)raw_data[4] << 8) | raw_data[5];
  data->temperature = ((float)raw_temperature / (1 << 20)) * 200.0f - 50.0f;
  return 0;
}


static int aht10_drv_init(const struct device *dev)
{
  const struct aht10_config *config = dev->config;
  if (!device_is_ready(config->i2c.bus)) {
    LOG_ERR("I2C bus is not ready");
    return -ENODEV;
  }

  k_msleep(40); // AHT10 needs ≥20ms after power-on before accepting commands

  uint8_t status;
  int ret = i2c_read_dt(&config->i2c, &status, sizeof(status));
  if (ret < 0) {
    LOG_ERR("Failed to read AHT10 status (err %d) — check SDA=P1.02 SCL=P1.03", ret);
    return ret;
  }

  if (!(status & BIT(3))) {
    LOG_DBG("CAL bit not set, sending init command");
    uint8_t init_cmd[] = {AHT10_CMD_INIT, 0x08, 0x00};
    ret = i2c_write_dt(&config->i2c, init_cmd, sizeof(init_cmd));
    if (ret < 0) {
      LOG_ERR("Failed to send AHT10 init command (err %d)", ret);
      return ret;
    }
    k_msleep(10);
  }

  return 0;
}

static int aht10_drv_read_temperature(const struct device *dev, float *temperature)
{
  struct aht10_data *data = dev->data;
  read_raw(dev);
  // Read temperature data from the AHT10 sensor
  // This is a placeholder, actual I2C read commands should be implemented here
  LOG_DBG("Reading temperature from AHT10 sensor");
  *temperature = data->temperature; // Return the stored temperature value
  return 0;
}

static int aht10_drv_read_humidity(const struct device *dev, float *humidity)
{
  struct aht10_data *data = dev->data;
  read_raw(dev);
  // Read humidity data from the AHT10 sensor
  // This is a placeholder, actual I2C read commands should be implemented here
  LOG_DBG("Reading humidity from AHT10 sensor");
  *humidity = data->humidity; // Return the stored humidity value
  return 0;
}

// Define the AHT10 driver API structure
static const struct aht10_driver_api aht10_api = {
  .read_temperature = aht10_drv_read_temperature,
  .read_humidity = aht10_drv_read_humidity,
};

#define AHT10_INIT(n) \
    static const struct aht10_config aht10_config_##n = { \
        .i2c = I2C_DT_SPEC_INST_GET(n), \
    }; \
    static struct aht10_data aht10_data_##n; \
    DEVICE_DT_INST_DEFINE(n, aht10_drv_init, NULL, \
                          &aht10_data_##n, &aht10_config_##n, \
                          POST_KERNEL, CONFIG_AHT10_INIT_PRIORITY, \
                          &aht10_api);

DT_INST_FOREACH_STATUS_OKAY(AHT10_INIT)