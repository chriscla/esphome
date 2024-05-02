import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
#from components import a7105
from esphome.const import (
    CONF_OUTPUT_ID,
    CONF_CHANNEL,
    CONF_GROUP,
    CONF_DEFAULT_TRANSITION_LENGTH,
    CONF_COLD_WHITE_COLOR_TEMPERATURE,
    CONF_WARM_WHITE_COLOR_TEMPERATURE,
)

DEPENDENCIES = ["a7105"]

# This is what other objects would use to refer to this.
#CONF_NEEWER_LIGHT = "neewer_light_id"


light_ns = cg.esphome_ns.namespace("neewer_light")
NEEWER_LIGHT = light_ns.class_("neewer_light", cg.Component, light.LightOutput)

CONFIG_SCHEMA = cv.All(
    light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
     {
       cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(NEEWER_LIGHT),
       # cv.GenerateID(a7105.CONF_A7105_ID) : cv.use_id(a7105.A7105Component),
       cv.Required(CONF_CHANNEL): cv.uint8_t,
       cv.Optional(CONF_GROUP, 1): cv.uint8_t,
       cv.Required(CONF_COLD_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
       cv.Required(CONF_WARM_WHITE_COLOR_TEMPERATURE): cv.color_temperature,
        }
    ).extend(cv.COMPONENT_SCHEMA),
    light.validate_color_temperature_channels
)

async def to_code(config):

    # This light can't transition
    config[CONF_DEFAULT_TRANSITION_LENGTH] = 0

    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    print(config)
    await cg.register_component(var, config)
    await light.register_light(var, config)

    cg.add(var.set_channel(config[CONF_CHANNEL]))    
    cg.add(var.set_group(config[CONF_GROUP]))    
    cg.add(var.set_cold_white_temperature(config[CONF_COLD_WHITE_COLOR_TEMPERATURE]))
    cg.add(var.set_warm_white_temperature(config[CONF_WARM_WHITE_COLOR_TEMPERATURE]))

   # paren = await cg.get_variable(config[a7105.CONF_A7105_ID])
    #cg.add(var.set_a7105(paren))
    
    # if core.is_esp32:
        # cg.add_library("SPI", None)