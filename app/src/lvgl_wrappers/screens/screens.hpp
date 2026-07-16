#pragma once

#include <lvgl.h>
#include "ble/ble.hpp"
#include "ble/aht10_service.hpp"
#include "mpu6050/mpu6050.hpp"

class Screen {
public:
  virtual ~Screen() = default;
  virtual void show();
  virtual void hide();
  virtual bool isActive();
  virtual void update();
};

class ScreenBLE : public Screen {

public:
  ScreenBLE(BLE &ble, AHT10Service &ahtService);
  ~ScreenBLE() override;
  void show() override;
  void hide() override;
  void update() override;
  bool isActive() override;

private:
  uint8_t notifyCount{0};
  uint8_t displayedNotifyCount{0};
  BLE &ble;
  AHT10Service &ahtService;
  bool connected{true};
  bool humiditySubscribed{false};
  bool temperatureSubscribed{false};

  void updateConnectionStatus(BLE &ble);
  void updateNotifyTemperature(AHT10Service &service);
  void updateNotifyHumidity(AHT10Service &service);
  void updateNotifyCount(void);
  void updateRSSI(void);
};

class ScreenClimate : public Screen {

public:
  ScreenClimate(AHT10 &aht, BLE &ble);
  ~ScreenClimate() override;
  void show() override;
  void hide() override;
  void update() override;
  bool isActive() override;

private:
  AHT10 &aht;
  BLE &ble;
  bool connected{true};

  void updateConnectionStatus();
};

class ScreenMotion : public Screen {

public:
  ScreenMotion(MPU6050 &mpu, BLE &ble);
  ~ScreenMotion() override;
  void show() override;
  void hide() override;
  void update() override;
  bool isActive() override;

private:
  MPU6050 &mpu;
  BLE &ble;
  bool connected{true};

  void updateConnectionStatus();
};