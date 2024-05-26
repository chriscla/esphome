#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/ddc/ddc_device.h"

#define DDC_BRIGHTNESS_ID 0x10

namespace esphome {
namespace ddc {

class DDCSensor : 
  public PollingComponent {

 public:
  
  void set_ddc_device(DDCDevice *ddc_device) { this->ddc_device_ = ddc_device; }
  void add_brightness_sensor(sensor::Sensor *brightness_sensor) { this->brightness_sensor_ = brightness_sensor; }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
    DDCDevice *ddc_device_{nullptr};
    sensor::Sensor *brightness_sensor_{nullptr};
  }; //end class
}  // namespace ddccontrol
}  // namespace esphome