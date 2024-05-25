import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core
from esphome.components import sensor
from .. import ddc_ns, DDCDevice
from esphome.const import (
    CONF_ID,
    CONF_BRIGHTNESS,
    CONF_UNIT_OF_MEASUREMENT,
    UNIT_LUX,
    DEVICE_CLASS_ILLUMINANCE,
    STATE_CLASS_MEASUREMENT
)

DEPENDENCIES = ["hdmiddc"]

CONF_DDC_ID = "ddc_id"
DDCSensorComponent = ddc_ns.class_("DDCSensor", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema( 
    {
        cv.GenerateID(): cv.declare_id(DDCSensorComponent),
        cv.GenerateID(CONF_DDC_ID): cv.use_id(DDCDevice),
        cv.Optional(CONF_BRIGHTNESS): sensor.sensor_schema(
            unit_of_measurement=UNIT_LUX,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_ILLUMINANCE,
            state_class=STATE_CLASS_MEASUREMENT
        )
    }
    ).extend(cv.polling_component_schema("5s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    ddc_bus = await cg.get_variable(config[CONF_DDC_ID])
    cg.add(var.set_ddc_device(ddc_bus))
    if CONF_BRIGHTNESS in config:
        sens = await sensor.new_sensor(config[CONF_BRIGHTNESS])
        cg.add(var.add_brightness_sensor(sens))
    
    #await cg.register_sensor(var, config[CONF_BRIGHTNESS], ddc_bus.brightness_sensor)
    #cg.add(var.set_brightness_sensor(sens))
    #await sensor.register_sensor(var, config)
