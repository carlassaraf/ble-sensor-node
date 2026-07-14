#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <aht10/aht10.hpp>
#include <ble/ble.hpp>
#include <ble/aht10_service.hpp>
#include <lvgl_wrappers/lvgl_port.hpp>
#include <lvgl_wrappers/screens/screens.hpp>
#include <lvgl_wrappers/ui.hpp>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

int main(void)
{

    if(!gpio_is_ready_dt(&btn)) {
        LOG_ERR("Button not ready");
        return -1;
    }
    if(gpio_pin_configure_dt(&btn, GPIO_INPUT) != 0) {
        LOG_ERR("Error with GPIO configuration");
        return -1;
    }
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

    UI ui(ble, service, aht10, btn);
    ui.run();
    return 0;
}
