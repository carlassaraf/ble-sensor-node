#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

namespace BoardLEDs {
  inline const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
  inline const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
  inline const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
  inline const struct gpio_dt_spec led4 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);
}

class LED {
public:
  LED(const struct gpio_dt_spec &gpio) : _spec(gpio) {
    if(!gpio_is_ready_dt(&_spec)) {
      return;
    }
    gpio_pin_configure_dt(&_spec, GPIO_OUTPUT_INACTIVE);
  }
  /** @brief Read LED status */
  bool get() { return gpio_pin_get_dt(&_spec) == 1; }
  /** @brief Turns LED on */
  void on() { gpio_pin_set_dt(&_spec, 1); }
  /** @brief Turns LED off */
  void off() { gpio_pin_set_dt(&_spec, 0); }
  /** @brief Sets LED value */
  void set(bool value) { gpio_pin_set_dt(&_spec, (int)value); }

private:
  struct gpio_dt_spec _spec;
};