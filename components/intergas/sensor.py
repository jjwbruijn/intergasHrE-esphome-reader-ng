from esphome import automation
from esphome.automation import maybe_simple_id
import esphome.codegen as cg
from esphome.components import sensor, binary_sensor, uart, text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_DISABLED_BY_DEFAULT,

    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_EMPTY,

    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,

    UNIT_CELSIUS,
    UNIT_HOUR,

    ENTITY_CATEGORY_DIAGNOSTIC,
)
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["text_sensor", "binary_sensor"]

intergas_ns = cg.esphome_ns.namespace("intergas")
Intergas = intergas_ns.class_("Intergas", cg.PollingComponent, uart.UARTDevice)

SCH = lambda state_class, device_class, unit, sprecision: sensor.sensor_schema(
    unit_of_measurement=unit,
    accuracy_decimals=sprecision,
    device_class=device_class,
    state_class=state_class,
)

BSCH = lambda device_class="None": binary_sensor.binary_sensor_schema(
    device_class=device_class,
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Intergas),
            cv.Optional("heater_status_code", default={"name": "Status", "icon": "mdi:format-list-bulleted"}): text_sensor.text_sensor_schema(),
            cv.Optional("heater_dwk", default={"name": "Heater Zone", "icon": "mdi:radiator", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): text_sensor.text_sensor_schema(),
            cv.Optional("heater_last_fault_code", default={"name": "Last Fault Code", "icon": "mdi:alert-circle-outline", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): text_sensor.text_sensor_schema(),
            cv.Optional("heater_fault_code", default={"name": "Fault Code", "icon": "mdi:alert-circle-outline", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): text_sensor.text_sensor_schema(),
            cv.Optional("line_power_connected_count", default={"name": "Power Cycles", "icon": "mdi:power-plug-off", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_EMPTY,"", 0),
            cv.Optional("burner_start_count", default={"name": "Burner Start Count", "icon": "mdi:gas-burner", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_EMPTY,"", 0),
            cv.Optional("burner_starts_dhw_count", default={"name": "Burner Hot Water Start Count", "icon": "mdi:gas-burner", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_EMPTY,"", 0),
            cv.Optional("line_power_connected_hours", default={"name": "Powered On Hours", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_DURATION, UNIT_HOUR, 0),
            cv.Optional("ch_function_hours", default={"name": "Heating Hours", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_DURATION, UNIT_HOUR, 0),
            cv.Optional("dhw_function_hours", default={"name": "Hot Water Hours", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_DURATION, UNIT_HOUR, 0),
            cv.Optional("gas_meter_ch", default={"name": "Gas Usage Heating"}): SCH(STATE_CLASS_TOTAL_INCREASING, "gas", "m³", 3),
            cv.Optional("gas_meter_dhw", default={"name": "Gas Usage Hot Water"}): SCH(STATE_CLASS_TOTAL_INCREASING, "gas", "m³", 3),
            cv.Optional("ignition_failed", default={"name": "Ignition Failed Count", "icon": "mdi:fire-off", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_EMPTY,"", 0),
            cv.Optional("flame_lost", default={"name": "Flame Lost Count", "icon": "mdi:fire-off", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_EMPTY,"", 0),
            cv.Optional("reset_count", default={"name": "Reset Count", "icon": "mdi:restart", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_TOTAL_INCREASING, DEVICE_CLASS_EMPTY,"", 0),
            cv.Optional("water_meter", default={"name": "Water Usage"}): SCH(STATE_CLASS_TOTAL_INCREASING, "water", "m³", 3),
            cv.Optional("temperature_t1", default={"name": "Temperature T1"}): SCH(STATE_CLASS_MEASUREMENT, DEVICE_CLASS_TEMPERATURE, UNIT_CELSIUS, 2),
            cv.Optional("temperature_t2", default={"name": "Temperature T2"}): SCH(STATE_CLASS_MEASUREMENT, DEVICE_CLASS_TEMPERATURE, UNIT_CELSIUS, 2),
            cv.Optional("temperature_hot_water", default={"name": "Temperature Hot Water"}): SCH(STATE_CLASS_MEASUREMENT, DEVICE_CLASS_TEMPERATURE, UNIT_CELSIUS, 2),
            cv.Optional("temperature_setpoint", default={"name": "Temperature Setpoint"}): SCH(STATE_CLASS_MEASUREMENT, DEVICE_CLASS_TEMPERATURE, UNIT_CELSIUS, 2),
            cv.Optional("fanspeed_set", default={"name": "Fanspeed", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC, "icon": "mdi:fan"}): SCH(STATE_CLASS_MEASUREMENT, DEVICE_CLASS_EMPTY,"rpm", 0),
            cv.Optional("fanspeed", default={"name": "Fanspeed Actual", "icon": "mdi:fan"}): SCH(STATE_CLASS_MEASUREMENT, DEVICE_CLASS_EMPTY,"rpm", 0),
            cv.Optional("pressure", default={"name": "System Pressure"}): SCH(STATE_CLASS_MEASUREMENT, "pressure","bar", 2),
            cv.Optional("heater_io_current", default={"name": "IO Current", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): SCH(STATE_CLASS_MEASUREMENT, "current","µA", 3),
            cv.Optional("heater_pump_running", default={"name": "Pump", "icon": "mdi:pump"}): BSCH("running"),
            cv.Optional("heater_tap_switch", default={"name": "Hot Water", "icon":"mdi:water-pump", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH("running"),
            cv.Optional("heater_roomtherm", default={"name": "Room Thermostat", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
            cv.Optional("heater_alarm_status", default={"name": "Alarm Status", "icon": "mdi:alert-circle", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH("problem"),
            cv.Optional("heater_open_therm", default={"name": "OpenTherm Status", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
            cv.Optional("heater_cascade_relay", default={"name": "Cascade Relay", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
            cv.Optional("heater_gas_valve", default={"name": "Gas Valve", "icon": "mdi:valve", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH("opening"),
            cv.Optional("heater_spark", default={"name": "Ignition","icon": "mdi:creation", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
            cv.Optional("heater_ionisation_signal", default={"name": "Ionisation","icon": "mdi:fire", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
            cv.Optional("heater_ot_disabled", default={"name": "OT Disabled", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
            cv.Optional("heater_has_low_water_pressure", default={"name": "System Pressure","icon": "mdi:gauge-low", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH("problem"),
            cv.Optional("heater_burner_block", default={"name": "Burner Block", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
            cv.Optional("heater_gradient_flag", default={"name": "Gradient Flag", "entity_category": ENTITY_CATEGORY_DIAGNOSTIC}): BSCH(DEVICE_CLASS_EMPTY),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    SENSOR_KEYS = [
        ("line_power_connected_hours", True),
        ("line_power_connected_count", True),
        ("ch_function_hours", True),
        ("dhw_function_hours", True),
        ("burner_start_count", True),
        ("ignition_failed", True),
        ("flame_lost", True),
        ("reset_count", True),
        ("gas_meter_ch", False),
        ("gas_meter_dhw", False),
        ("water_meter", False),
        ("burner_starts_dhw_count", True),
        ("temperature_t1", False),
        ("temperature_t2", False),
        ("temperature_hot_water", False),
        ("temperature_setpoint", False),
        ("fanspeed_set", True),
        ("fanspeed", False),
        ("pressure", False),
        ("heater_io_current", True),
    ]
    for key, disabled in SENSOR_KEYS:
        await add_sensor(var, config, key, disabled)
    BINARY_SENSOR_KEYS = [
        ("heater_pump_running", False),
        ("heater_gp_switch", True),
        ("heater_tap_switch", True),
        ("heater_roomtherm", True),
        ("heater_alarm_status", False),
        ("heater_cascade_relay", True),
        ("heater_open_therm", True),
        ("heater_gas_valve", False),
        ("heater_spark", True),
        ("heater_ionisation_signal", True),
        ("heater_ot_disabled", True),
        ("heater_has_low_water_pressure", True),
        ("heater_burner_block", True),
        ("heater_gradient_flag", True),
    ]
    for key, disabled in BINARY_SENSOR_KEYS:
        await add_binary_sensor(var, config, key, disabled)

    TEXT_SENSOR_KEYS = [
        ("heater_status_code", False),
        ("heater_last_fault_code", False),
        ("heater_fault_code", False),
        ("heater_dwk", False),
    ]

    for key, disabled in TEXT_SENSOR_KEYS:
        await add_text_sensor(var, config, key, disabled)

async def add_text_sensor(var, config, conf_key, disabled_by_default):
    if conf := config.get(conf_key):
        conf = dict(conf)
        conf[CONF_DISABLED_BY_DEFAULT] = disabled_by_default
        sens = await text_sensor.new_text_sensor(conf)
        setter_name = f"set_{conf_key}"
        cg.add(getattr(var, setter_name)(sens))

async def add_binary_sensor(var, config, conf_key, disabled_by_default):
    if conf := config.get(conf_key):
        conf = dict(conf)
        conf[CONF_DISABLED_BY_DEFAULT] = disabled_by_default
        sens = await binary_sensor.new_binary_sensor(conf)
        setter_name = f"set_{conf_key}"
        cg.add(getattr(var, setter_name)(sens))

async def add_sensor(var, config, conf_key, disabled_by_default):
    if conf := config.get(conf_key):
        conf = dict(conf)
        conf[CONF_DISABLED_BY_DEFAULT] = disabled_by_default
        sens = await sensor.new_sensor(conf)
        setter_name = f"set_{conf_key}"
        cg.add(getattr(var, setter_name)(sens))
