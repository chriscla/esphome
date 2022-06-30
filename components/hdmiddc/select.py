import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, select
from esphome.const import (
    CONF_OPTIONS,
)

DEPENDENCIES = ["i2c"]

# This is what other objects would use to refer to this.
CONF_DDC_INPUT_SELECT = "ddc_input_select_id"

ddccontrol_ns = cg.esphome_ns.namespace("ddccontrol")
DDC_INPUT_SELECT = ddccontrol_ns.class_("DDCControlComponent", select.Select, i2c.I2CDevice, cg.Component)

def ensure_option_map(value):
    cv.check_not_templatable(value)
    option = cv.All(cv.int_range(0, 2**8 - 1))
    mapping = cv.All(cv.string_strict)
    options_map_schema = cv.Schema({option: mapping})
    value = options_map_schema(value)

    all_values = list(value.keys())
    unique_values = set(value.keys())
    if len(all_values) != len(unique_values):
        raise cv.Invalid("Mapping values must be unique.")

    return value


CONFIG_SCHEMA = select.SELECT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(DDC_INPUT_SELECT), 
        cv.Required(CONF_OPTIONS): ensure_option_map,       
    }
).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x37))

async def to_code(config):
    options_map = config[CONF_OPTIONS]
    var = await select.new_select(config, options=list(options_map.values()))
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_select_mappings(list(options_map.keys())))