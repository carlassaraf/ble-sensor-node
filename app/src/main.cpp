#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <aht10/aht10.hpp>
#include <ble/ble.hpp>
#include <ble/aht10_service.hpp>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
  const struct device *aht10_dev = DEVICE_DT_GET(DT_NODELABEL(aht10));
  AHT10 aht10(aht10_dev);
  if (!aht10.isInitialized()) {
    LOG_ERR("AHT10 device is not ready");
    return -1;
  }

  BLE ble;
  AHT10Service service;
  while(!ble.isEnabled()) {
    LOG_INF("Waiting for Bluetooth to be enabled...");
    k_msleep(100);
  }
  ble.startAdvertising();

  while(1) {
    float temp, humidity;
    int ret = aht10.readTemperature(temp);
    if (ret) {
      LOG_ERR("Failed to read temperature from AHT10 device (err %d)", ret);
      return -1;
    }
    ret = aht10.readHumidity(humidity);
    if (ret) {
      LOG_ERR("Failed to read humidity from AHT10 device (err %d)", ret);
      return -1;
    }
    LOG_INF("Temperature: %d.%.2d °C, Humidity: %d.%.2d %%", (int)temp, (int)(temp * 100) % 100, (int)humidity, (int)(humidity * 100) % 100);
    k_msleep(500);
  }
  return 0;
}
