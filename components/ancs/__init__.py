import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MAC_ADDRESS
from esphome.core import coroutine

DEPENDENCIES = ['esp32']
CONFLICTS_WITH = ['esp32_ble_tracker', 'esp32_ble_beacon']

ancs_ns = cg.esphome_ns.namespace('ancs')
ancs = ancs_ns.class_('ANCSComponent', cg.Component)

CONF_ANCS_ID = 'ancs_id'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ancs)
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)