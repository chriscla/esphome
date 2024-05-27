#include "ddc_select.h"

using namespace esphome;

namespace esphome
{
  namespace ddc
  {

    static const char *const TAG = "ddcselect";

    void DDCSelectComponent::dump_config()
    {
      LOG_SELECT("", "Input Select", this);
    }

    void DDCSelectComponent::control(const std::string &value)
    {
      ESP_LOGI(TAG, "Requested to control. New Value: %s", value.c_str());

      auto idx = this->index_of(value);
      if (idx.has_value())
      {
        uint8_t mapping = this->mappings_.at(idx.value());
        ESP_LOGI(TAG, "Setting datapoint value to 0x%02x:%s", mapping, value.c_str());
        this->ddc_device_->setVCP(0x60, mapping);
        return;
      }

      ESP_LOGW(TAG, "Invalid value %s", value.c_str());
    }

  } // end class
}