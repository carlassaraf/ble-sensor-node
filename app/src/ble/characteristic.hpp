#pragma once

#include <zephyr/types.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

template<typename T>
class Characteristic {
public:
  bool isSubscribed() { return subscribed; }
  void setSubscribed(bool sub) { subscribed = sub; }
  int notify(const struct bt_gatt_attr *attr)
  {
    if (!subscribed) {
		  return -EACCES;
	  }
  	return bt_gatt_notify(NULL, attr, &value, sizeof(value));
  }

private:
  T value{};
  bool subscribed{false};
};