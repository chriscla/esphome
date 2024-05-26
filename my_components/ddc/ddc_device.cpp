#include "ddc_device.h"

namespace esphome {
namespace ddc {

    static const char *const TAG = "ddcdevice";


    void DDCDevice::dump_config()
    {
      LOG_I2C_DEVICE(this);
    }



    // op: VCP opcode. Can be found with ddc capabilities
    // value: 16bit value
    void DDCDevice::setVCP(uint8_t op, uint16_t value)
    {

      ESP_LOGI(TAG, "Starting VCP Set.  op: 0x%02X  value: 0x%04X", op, value);

      // https://github.com/newAM/monitorcontrol/blob/main/monitorcontrol/vcp/vcp_linux.py
      uint8_t bytes[] =
      {
        0x51,
        0x84,
        0x03, // 0x03 for write, 0x02 for read
        op,  // VCP opcode
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

      ESP_LOGD(TAG, "Sending VCP Set: %s", format_hex_pretty(bytes, sizeof(bytes)).c_str());
      auto err = this->write(bytes, sizeof(bytes));
      if (err != i2c::ERROR_OK)
      {
        ESP_LOGW(TAG, "VCP Set write failed. i2c Error Code: %d", err);
      }

      ESP_LOGI(TAG, "Finished VCP Set");
    }

    // op: VCP opcode. Can be found with ddc capabilities
    // value: 16bit value
    void DDCDevice::getVCP(uint8_t op,  std::function<void(uint16_t)> result_callback)
    {

      ESP_LOGI(TAG, "Starting VCP Get.  op: 0x%02X", op);

      uint8_t bytes[] =
        {
            0x51,  // Host Address
            0x82,
            0x01,
            op,
            (uint8_t)((this->address_ << 1) ^ 0x51 ^ 0x82 ^ 0x01 ^ op)
        }; 

      ESP_LOGD(TAG, "Sending VCP Get: %s", format_hex_pretty(bytes, sizeof(bytes)).c_str());
      auto write_err = this->write(bytes, sizeof(bytes), true);
      if (write_err != i2c::ERROR_OK)
      {
        ESP_LOGW(TAG, "VCP Get write failed. op: 0x%02x  err: %d", op, write_err);
        return;
      }

      this->set_timeout("VCPGet", 4500, [this, result_callback]() {
        uint8_t response[12];
        auto read_err = this->read(response, sizeof(response));
        if (read_err != i2c::ERROR_OK)
        {
          ESP_LOGW(TAG, "VCP Get Read Failed. err: %d", read_err);
          return;
        }

        uint16_t result = (response[8] << 8) + response[9];
        ESP_LOGI(TAG, "Finished VCP Get.  Response: 0x%04x", result);
        
        result_callback(result);
      });
    }

  } // end class
}
