#pragma once
#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/a7105/a7105.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"

#define NEEWER_ID 0xD253694D

#define NEEWER_CH1 0x3
#define NEEWER_CH2 0x7

namespace esphome {
namespace light {

class NeewerLight : 
  public Component, public LightOutput {

 public:
  
  void setup() override;

  public: 
    /**
     * Write the state to this light
     */
    void write_state(light::LightState *state) override;
    light::LightTraits get_traits() override;
    void set_a7105(a7105::A7105 *a7105_chip) { this->a7105_chip_ = a7105_chip; }
    void set_channel(uint8_t channel) { this->channel_ = channel; }
    void set_group(uint8_t group) { this->group_ = group; }

    void set_cold_white_temperature(float cold_white_temperature) { cold_white_temperature_ = cold_white_temperature; }
    void set_warm_white_temperature(float warm_white_temperature) { warm_white_temperature_ = warm_white_temperature; }

  protected:
    void send_command(const std::vector<uint8_t> &data);
  
    a7105::A7105 *a7105_chip_;
    uint8_t channel_;
    uint8_t group_;
    float cold_white_temperature_;
    float warm_white_temperature_;
    int8_t power_state_;
  
  }; //end class
}  // namespace light
}  // namespace esphome