#pragma once
#include "esphome/core/component.h"
#include "esphome/components/select/select.h"
#include "esphome/components/ddc/ddc_device.h"

namespace esphome {
namespace ddc {

class DDCSelectComponent:
  public select::Select,
  public Component {

 public:
  
  void set_ddc_device(DDCDevice *ddc_device) { this->ddc_device_ = ddc_device; }
  void set_select_mappings(std::vector<uint8_t> mappings) { this->mappings_ = std::move(mappings); }

  void control(const std::string &value) override;
  void dump_config() override;

 protected:
    DDCDevice *ddc_device_{nullptr};
    std::vector<uint8_t> mappings_;

  }; //end class
}  // namespace ddccontrol
}  // namespace esphome