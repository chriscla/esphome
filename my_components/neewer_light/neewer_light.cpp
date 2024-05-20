#include "neewer_light.h"

using namespace esphome;

namespace esphome {
namespace neewer_light {

static const char *const TAG = "NeewerLight";


void NeewerLight::setup() {

    ESP_LOGCONFIG(TAG, "NeewerLight setup");
    ESP_LOGI(TAG, "Starting setup");

    uint8_t res = this->a7105_chip_->reset();

    //uint8_t channel = 0x3;

    this->a7105_chip_->set_id(NEEWER_ID);
    this->a7105_chip_->write_reg(a7105::A7105_0F_CHANNEL, this->channel_);

    this->a7105_chip_->write_reg(a7105::A7105_01_MODE_CONTROL, 0x42);
    this->a7105_chip_->write_reg(a7105::A7105_02_CALC, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_03_FIFOI, 0x05);
    this->a7105_chip_->write_reg(a7105::A7105_04_FIFOII, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_07_RC_OSC_I, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_08_RC_OSC_II, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_09_RC_OSC_III, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_0A_CK0_PIN, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_0B_GPIO1_PIN_I, 0x01);
    this->a7105_chip_->write_reg(a7105::A7105_0C_GPIO2_PIN_II, 0x09);
    this->a7105_chip_->write_reg(a7105::A7105_0D_CLOCK, 0x05);
    this->a7105_chip_->write_reg(a7105::A7105_0E_DATA_RATE, 0x04);

    this->a7105_chip_->write_reg(a7105::A7105_10_PLL_II, 0x9E);
    this->a7105_chip_->write_reg(a7105::A7105_11_PLL_III, 0x4B);
    this->a7105_chip_->write_reg(a7105::A7105_12_PLL_IV, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_13_PLL_V, 0x02);
    this->a7105_chip_->write_reg(a7105::A7105_14_TX_I, 0x16);
    this->a7105_chip_->write_reg(a7105::A7105_15_TX_II, 0x2B);
    this->a7105_chip_->write_reg(a7105::A7105_16_DELAY_I, 0x12);
    this->a7105_chip_->write_reg(a7105::A7105_17_DELAY_II, 0x00);

    this->a7105_chip_->write_reg(a7105::A7105_18_RX, 0x62);
    this->a7105_chip_->write_reg(a7105::A7105_19_RX_GAIN_I, 0x80);
    this->a7105_chip_->write_reg(a7105::A7105_1A_RX_GAIN_II, 0x80);
    this->a7105_chip_->write_reg(a7105::A7105_1B_RX_GAIN_III, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_1C_RX_GAIN_IV, 0x0A);

    this->a7105_chip_->write_reg(a7105::A7105_1D_RSSI_THOLD, 0x32);
    this->a7105_chip_->write_reg(a7105::A7105_1E_ADC, 0xC3);
    this->a7105_chip_->write_reg(a7105::A7105_1F_CODE_I, 0x07);
    this->a7105_chip_->write_reg(a7105::A7105_20_CODE_II, 0x16);
    this->a7105_chip_->write_reg(a7105::A7105_21_CODE_III, 0x00);

    this->a7105_chip_->write_reg(a7105::A7105_22_IF_CALIB_I, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_24_VCO_CURCAL, 0x13);
    this->a7105_chip_->write_reg(a7105::A7105_25_VCO_SBCAL_I, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_26_VCO_SBCAL_II, 0x3B);
    this->a7105_chip_->write_reg(a7105::A7105_27_BATTERY_DET, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_28_TX_TEST, 0x17);
    this->a7105_chip_->write_reg(a7105::A7105_29_RX_DEM_TEST_I, 0x07);
    this->a7105_chip_->write_reg(a7105::A7105_2A_RX_DEM_TEST_II, 0x80);

    this->a7105_chip_->write_reg(a7105::A7105_2B_CPC, 0x03);
    this->a7105_chip_->write_reg(a7105::A7105_2C_XTAL_TEST, 0x01);
    this->a7105_chip_->write_reg(a7105::A7105_2D_PLL_TEST, 0x45);

    this->a7105_chip_->write_reg(a7105::A7105_2E_VCO_TEST_I, 0x18);
    this->a7105_chip_->write_reg(a7105::A7105_2F_VCO_TEST_II, 0x00);
    this->a7105_chip_->write_reg(a7105::A7105_30_IFAT, 0x01);
    this->a7105_chip_->write_reg(a7105::A7105_31_RSCALE, 0x0F);

        //VCO Calibration
    this->a7105_chip_->write_reg(a7105::A7105_02_CALC, 0x1);
    unsigned long ms = millis();
    while(millis() - ms < 500) {
        uint8_t calc_value = 0;
        this->a7105_chip_->read_reg(a7105::A7105_02_CALC, &calc_value);
        //TODO: Look this is mis(ing curly braces!!!!!! Clear bug
        if(!calc_value)
            break;
    }
    if (millis() - ms >= 500) {
        return;
    }
  //  this->a7105_chip_->writeRe(a7105::ister(A7105_0F_PLL_I, 0x07);

    // It looks like the origina(a7105:: code polls the calibration control register

    this->a7105_chip_->write_reg(a7105::A7105_22_IF_CALIB_I, 0x07);
    this->a7105_chip_->write_reg(a7105::A7105_24_VCO_CURCAL, 0x13);
    this->a7105_chip_->write_reg(a7105::A7105_25_VCO_SBCAL_I, 0x09);

    this-> power_state_ = -1; //-1 == UNKNOWN

    ESP_LOGI(TAG, "Setup complete.");
}

  light::LightTraits NeewerLight::get_traits()  {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE});
    traits.set_min_mireds(this->cold_white_temperature_);
    traits.set_max_mireds(this->warm_white_temperature_);
    return traits;
  }

  void NeewerLight::write_state(light::LightState *state) {
    bool light_state;
    state->current_values_as_binary(&light_state);

    if (light_state) {

      float brightness, color_temperature;
      //state->current_values_as_ct(&color_temperature, &brightness);
      
      uint8_t converted_brightness = (uint8_t)(state->remote_values.get_brightness() * 100);
      color_temperature = state->remote_values.get_color_temperature();

      //Convert from minred to Kelvin      
      uint16_t kelvin = round(1000000.0f / color_temperature);
      uint8_t color_temperature_code = (uint8_t)(kelvin / 100);

      //round up the hundreds
      if (kelvin % 100 > 50) {
        color_temperature_code += 1;
      }

      ESP_LOGD(TAG, "C: 0x%02x G: 0x%02x Brightness: 0x%02x CT Dec: %d", 
        this->channel_, 
        this->group_, 
        converted_brightness, 
        color_temperature_code
      );

      // Do we think the light is on?
      if (this->power_state_ <= 0) {
        // This turns on the light if it's off
        std::vector<uint8_t> restore_command = {0x7E, 0xA4, this->group_, 0x01, 0x00};
        this->send_command(restore_command);
        this->power_state_ = 1; //Remember that the light is on. 
      }

      std::vector<uint8_t> color_set_command = {0x7E, 0xA5, this->group_, converted_brightness, color_temperature_code};
      this->send_command(color_set_command);

    } else {

      //Remember that we are off.
      this->power_state_ = 0;

      std::vector<uint8_t> off_command = {0x7E, 0xA4, this->group_, 0x00, 0x00};
      this->send_command(off_command);
    }
  }

  void NeewerLight::send_command(const std::vector<uint8_t> &data) {
      
  std::vector<uint8_t> checksum_data;
  uint8_t checksum = 0;

  for (auto j : data) {
    checksum += j;  
  }
  checksum = checksum % 256;

  checksum_data = data;
  checksum_data.push_back(checksum);

  ESP_LOGD(TAG, "Sending command: %s", format_hex_pretty(checksum_data).c_str());    

  for (int num_repeats=0; num_repeats < 4; num_repeats++) {
    for (int i=0; i<4;i++) {
          this->a7105_chip_->write_data(checksum_data, this->channel_);
        }
  }

} // end class
}
}