#include "ddc_sensor.h"

using namespace esphome;

namespace esphome
{
  namespace ddc
  {

    static const char *const TAG = "ddcsensor";

    void DDCSensor::setup()
    {
      ESP_LOGD(TAG, "Setting up DDC Sensor");
      this->ddc_device_->setup();
      //this->brightness_sensor_->setup();
    }

    void DDCSensor::dump_config()
    {
      //LOG_I2C_DEVICE(this->ddc_device_)
      LOG_SENSOR("", "Brightness", this->brightness_sensor_);
    }

    void DDCSensor::update()
    {
      ESP_LOGD(TAG, "Updating DDC Sensor");
      uint16_t brightness = this->ddc_device_->getVCP(DDC_BRIGHTNESS_ID);
      ESP_LOGI(TAG, "Brightness: %d", brightness);
      this->brightness_sensor_->publish_state(brightness);
    }

  } // end class
}