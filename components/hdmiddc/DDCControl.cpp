#include "DDCControl.h"

namespace esphome
{
  namespace ddccontrol
  {

    static const char *const TAG = "ddccontrol";

    void DDCControlComponent::setup()
    {

      ESP_LOGCONFIG(TAG);
      ESP_LOGI(TAG, "Starting ddccontrol setup");

      this->set_i2c_address(0x37);
      // uint8_t channel = 0x3;
      Wire.begin();

      ESP_LOGI(TAG, "Setup complete.");
    }

    void DDCControlComponent::control(const std::string &value)
    {

      ESP_LOGI(TAG, "Requested to control. New Value: %s", value.c_str());

      auto idx = this->index_of(value);
      if (idx.has_value())
      {
        uint8_t mapping = this->mappings_.at(idx.value());
        ESP_LOGI(TAG, "Setting datapoint value to 0x%02x:%s", mapping, value.c_str());
        this->setVCP(0x60, mapping);
        return;
      }

      ESP_LOGW(TAG, "Invalid value %s", value.c_str());
    }

    // op: VCP opcode. Can be found with ddc capabilities
    // value: 16bit value
    void DDCControlComponent::setVCP(uint8_t op, uint16_t value)
    {

      ESP_LOGI(TAG, "Starting VCP Set.  op: 0x%02X  value: 0x%04X", op, value);

      uint8_t bytes[] =
      {
        0x51,
        0x84,
        0x03,
        op,
        (uint8_t)(value >> 8),
        (uint8_t)value,
        0
      };

      uint8_t checksum = 0x37 << 1;
      for (int i = 0; i < 6; i++)
      {
        checksum ^= bytes[i];
      }
      bytes[6] = checksum;

      if (auto err = this->write(bytes, sizeof(bytes)) != i2c::ERROR_OK)
      {
        ESP_LOGW(TAG, "VCP Set write failed.");
      }

      ESP_LOGI(TAG, "Finished VCP Set");
    }

    // op: VCP opcode. Can be found with ddc capabilities
    // value: 16bit value
    uint16_t DDCControlComponent::getVCP(uint8_t op)
    {

      ESP_LOGI(TAG, "Starting VCP Get.  op: 0x%02X", op);

      uint8_t bytes[] =
          {
              0x51,
              0x82,
              0x01,
              op,
              (uint8_t)((0x37 << 1) ^ 0x51 ^ 0x82 ^ 0x01 ^ op)};

      if (auto err = this->write(bytes, sizeof(bytes)) != i2c::ERROR_OK)
      {
        ESP_LOGW(TAG, "VCP Get write failed. op: 0x%02x", op);
        return 0;
      }

      ESP_LOGI(TAG, "Delaying before Read.");
      esphome::delay_microseconds_safe(45000);

      uint8_t response[12];
      if (this->read(response, sizeof(response)) != i2c::ERROR_OK)
      {
        ESP_LOGW(TAG, "VCP Get Read Failed: op: 0x%02x", op);
        return 0;
      }

      uint16_t result = (response[8] << 8) + response[9];
      ESP_LOGI(TAG, "Finished VCP Get.  Response: 0x%04x", result);
      
      return result;
    }
  } // end class
}
