#pragma once
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "ddc_device.h"


#define DDC_BRIGHTNESS_ID 0x10

namespace esphome {
namespace ddc {


class DDCSensor : 
  public PollingComponent, public DDCDevice {

 public:
  
  void set_brightness_sensor(sensor::Sensor *brightness_sensor) { brightness_sensor_ = brightness_sensor; }

  void update() override;
  void dump_config() override;

 protected:
  sensor::Sensor *brightness_sensor_{nullptr};
  }; //end class
}  // namespace ddccontrol
}  // namespace esphome