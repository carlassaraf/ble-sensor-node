/**
 * Copyright (c) 2024 Iomico, Inc.
 * SPDX-License-Identifier: Apache-2.0
 * 
 * This file's intent is to provide a C++ wrapper around the C API defined in aht10.h, allowing 
 * for more idiomatic C++ usage while still leveraging the underlying Zephyr device model and API.
 */

#pragma once

#include <aht10.h>

class AHT10 {
public:
  explicit AHT10(const struct device *device) : dev(device), initialized(device_is_ready(device)) { }

  int readTemperature(float &temperature) {
    return aht10_read_temperature(dev, &temperature);
  }

  int readHumidity(float &humidity) {
    return aht10_read_humidity(dev, &humidity);
  }

  bool isInitialized() const {
    return initialized;
  }

private:
  const struct device *dev;
  bool initialized;
};