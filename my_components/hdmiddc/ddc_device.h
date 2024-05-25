#pragma once
#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ddc {

class DDCDevice :
  public Component,
  public i2c::I2CDevice {

 public:
  
  //void dump_config() override;
  //void set_select_mappings(std::vector<uint8_t> mappings) { this->mappings_ = std::move(mappings); }

  void setVCP(uint8_t op, uint16_t value);
  uint16_t getVCP(uint8_t op);
    //void control(const std::string &value) override;

    //std::vector<uint8_t> mappings_;
  }; //end class
}  // namespace ddccontrol
}  // namespace esphome