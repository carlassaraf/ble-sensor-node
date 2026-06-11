#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <aht10/aht10.hpp>
#include <ble/ble.hpp>
#include <ble/aht10_service.hpp>
#include <lvgl_wrappers/lvgl_port.hpp>
#include <lvgl_wrappers/screens.hpp>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    const struct device *aht10_dev = DEVICE_DT_GET(DT_NODELABEL(aht10));
    AHT10 aht10(aht10_dev);
    if (!aht10.isInitialized()) {
        LOG_ERR("AHT10 device is not ready");
        return -1;
    }

    BLE ble(LED(BoardLEDs::led2), LED(BoardLEDs::led3), LED(BoardLEDs::led4));
    AHT10Service service(aht10, LED(BoardLEDs::led1));
    service.start();

    while (!ble.isEnabled()) {
        k_msleep(10);
    }
    ble.startAdvertising();

    LVGL lvgl;
    lvgl.start();

    ScreenBLE scrBLE;

    while (1) {
        float temp, hum;
        aht10.readTemperature(temp);
        aht10.readHumidity(hum);
        lvgl.lock();
        scrBLE.update(temp, hum);
        lvgl.unlock();
        k_msleep(100);
    }

    return 0;
}
