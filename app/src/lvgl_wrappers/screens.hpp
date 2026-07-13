#include <lvgl.h>
#include "ble/ble.hpp"

class Screen {
public:
  virtual ~Screen() = default;
  virtual void show();
  virtual void hide();
};

class ScreenBLE : public Screen {

public:
  ~ScreenBLE() override;
  void show() override;
  void update(BLE &ble);
  void hide() override;

private:
  bool connected{true};
};