#pragma once
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

#define DDC_BRIGHTNESS_ID 0x10

namespace esphome {
namespace ddc {

class DDCDevice :
  public Component,
  public i2c::I2CDevice {

 public:
  
  void dump_config() override;

  void setVCP(uint8_t op, uint16_t value);
  void getVCP(uint8_t op, std::function<void(uint16_t)> result_callback);

  }; //end class
}  // namespace ddccontrol
}  // namespace esphome