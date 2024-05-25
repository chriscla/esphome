import esphome.codegen as cg
from esphome.components import i2c

ddc_ns = cg.esphome_ns.namespace("ddc")
DDCBase = ddc_ns.class_("DDCDevice", i2c.I2CDevice)