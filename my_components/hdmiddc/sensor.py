import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core
from esphome.components import i2c, sensor
from . import ddc_ns, DDCBase
from esphome.const import (
    CONF_ID,
    CONF_UNIT_OF_MEASUREMENT,
    UNIT_LUX
)

DEPENDENCIES = ["i2c"]

# This is what other objects would use to refer to this.
CONF_DDC_SENSOR = "ddc_brightness_id"

DDCSensorComponent = ddc_ns.class_("DDCSensor", i2c.I2CDevice, cg.PollingComponent, DDCBase )

CONFIG_SCHEMA = (
    sensor.sensor_schema(
    DDCSensorComponent,
    unit_of_measurement=UNIT_LUX,
    accuracy_decimals=0
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(cv.polling_component_schema("5s"))
    .extend(i2c.i2c_device_schema(0x37))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    sens = await sensor.new_sensor(config)
    #cg.add(var.set_brightness_sensor(sens))
    #await sensor.register_sensor(var, config)
