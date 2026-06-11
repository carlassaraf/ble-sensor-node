#include <lvgl.h>

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
  void update(float &temp, float &hum);
  void hide() override;
};