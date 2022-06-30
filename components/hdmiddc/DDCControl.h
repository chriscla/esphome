#pragma once
#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/components/select/select.h"
#include "esphome/components/i2c/i2c.h"


namespace esphome {
namespace ddccontrol {

class DDCControlComponent : 
  public Component, public select::Select, public i2c::I2CDevice {

 public:
  
  void setup() override;
  void set_select_mappings(std::vector<uint8_t> mappings) { this->mappings_ = std::move(mappings); }

  public: 

  protected:  
    void control(const std::string &value) override;
    void setVCP(uint8_t op, uint16_t value);
    uint16_t getVCP(uint8_t op);

    std::vector<uint8_t> mappings_;
  }; //end class
}  // namespace ddccontrol
}  // namespace esphome