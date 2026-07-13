#include <lvgl.h>
#include "ble/ble.hpp"
#include "ble/aht10_service.hpp"

class Screen {
public:
  virtual ~Screen() = default;
  virtual void show();
  virtual void hide();
  virtual bool isActive();
};

class ScreenBLE : public Screen {

public:
  ~ScreenBLE() override;
  void show() override;
  void update(BLE &ble, AHT10Service &service);
  void hide() override;
  bool isActive() override;

private:
  uint8_t notifyCount{0};
  uint8_t displayedNotifyCount{0};  
  bool connected{true};
  bool humiditySubscribed{false};
  bool temperatureSubscribed{false};

  void updateConnectionStatus(BLE &ble);
  void updateNotifyTemperature(AHT10Service &service);
  void updateNotifyHumidity(AHT10Service &service);
  void updateNotifyCount(void);
};

class ScreenClimate : public Screen {

public:
  ~ScreenClimate() override;
  void show() override;
  void update(AHT10 &aht);
  void hide() override;
  bool isActive() override;
};