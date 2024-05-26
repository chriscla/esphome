import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from .. import ddc_ns, DDCDevice
from esphome.const import (
    CONF_OPTIONS,
)

DEPENDENCIES = ["ddc"]
CONF_DDC_ID = "ddc_id"

# This is what other objects would use to refer to this.
CONF_DDC_INPUT_SELECT = "ddc_input_select_id"

DDCSelectComponent = ddc_ns.class_("DDCSelectComponent", select.Select, cg.Component)

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
        cv.GenerateID(): cv.declare_id(DDCSelectComponent),
        cv.GenerateID(CONF_DDC_ID): cv.use_id(DDCDevice),
        cv.Required(CONF_OPTIONS): ensure_option_map,       
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    options_map = config[CONF_OPTIONS]
    var = await select.new_select(config, options=list(options_map.values()))
    ddc_bus = await cg.get_variable(config[CONF_DDC_ID])
    cg.add(var.set_ddc_device(ddc_bus))
    await cg.register_component(var, config)
    cg.add(var.set_select_mappings(list(options_map.keys())))