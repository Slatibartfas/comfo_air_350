
from esphome.components import uart
import esphome.codegen as cg
import esphome.config_validation as cv

comfoair_bridge_ns = cg.esphome_ns.namespace('comfoair_bridge')
ComfoAirBridge = comfoair_bridge_ns.class_('ComfoAirBridge', cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema({
        cv.GenerateID(): cv.declare_id(ComfoAirBridge),
    })
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.polling_component_schema('30s'))
)

async def to_code(config):
    var = cg.new_Pvariable(config["id"])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
