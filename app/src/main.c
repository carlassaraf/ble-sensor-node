#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <aht10.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
  const struct device *aht10_dev = DEVICE_DT_GET(DT_NODELABEL(aht10));
  if(!device_is_ready(aht10_dev)) {
    LOG_ERR("AHT10 device is not ready");
    return -1;
  }

  while(1) {
    float temp, humidity;
    int ret = aht10_read_temperature(aht10_dev, &temp);
    if (ret) {
      LOG_ERR("Failed to read temperature from AHT10 device (err %d)", ret);
      return -1;
    }
    ret = aht10_read_humidity(aht10_dev, &humidity);
    if (ret) {
      LOG_ERR("Failed to read humidity from AHT10 device (err %d)", ret);
      return -1;
    }
    LOG_INF("Temperature: %d.%.2d °C, Humidity: %d.%.2d %%", (int)temp, (int)(temp * 100) % 100, (int)humidity, (int)(humidity * 100) % 100);
    k_msleep(500);
  }
  return 0;
}
