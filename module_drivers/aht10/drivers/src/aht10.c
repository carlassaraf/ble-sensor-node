#define DT_DRV_COMPAT aht10

#include <zephyr/device.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "aht10.h"

LOG_MODULE_REGISTER(aht10, CONFIG_AHT10_LOG_LEVEL);