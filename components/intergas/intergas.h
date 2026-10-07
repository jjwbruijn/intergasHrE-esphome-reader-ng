#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace intergas {

#define DEFINE_SENSOR(name)           \
    sensor::Sensor *name##_{nullptr}; \
    void set_##name(sensor::Sensor *s) { name##_ = s; }
#define DEFINE_BSENSOR(name)                       \
    binary_sensor::BinarySensor *name##_{nullptr}; \
    void set_##name(binary_sensor::BinarySensor *s) { name##_ = s; }
#define DEFINE_TSENSOR(name)                   \
    text_sensor::TextSensor *name##_{nullptr}; \
    void set_##name(text_sensor::TextSensor *s) { name##_ = s; }

class Intergas : public PollingComponent, public uart::UARTDevice {
   public:
    float get_setup_priority() const override;
    void setup() override;
    void update() override;
    void dump_config() override;

    void readData(uint8_t *buffer, uint8_t len);
    bool processData(uint8_t type, uint8_t len, uint8_t *buffer);

    DEFINE_SENSOR(line_power_connected_count)
    DEFINE_SENSOR(line_power_connected_hours)
    DEFINE_SENSOR(ch_function_hours)
    DEFINE_SENSOR(dhw_function_hours)
    DEFINE_SENSOR(burner_start_count)
    DEFINE_SENSOR(ignition_failed)
    DEFINE_SENSOR(flame_lost)
    DEFINE_SENSOR(reset_count)
    DEFINE_SENSOR(gas_meter_ch)
    DEFINE_SENSOR(gas_meter_dhw)
    DEFINE_SENSOR(water_meter)
    DEFINE_SENSOR(burner_starts_dhw_count)
    DEFINE_SENSOR(temperature_t1)
    DEFINE_SENSOR(temperature_t2)
    DEFINE_SENSOR(temperature_hot_water)
    DEFINE_SENSOR(temperature_setpoint)
    DEFINE_SENSOR(fanspeed_set)
    DEFINE_SENSOR(fanspeed)
    DEFINE_SENSOR(pressure)
    DEFINE_SENSOR(heater_io_current)

    DEFINE_TSENSOR(heater_status_code)
    DEFINE_TSENSOR(heater_fault_code)
    DEFINE_TSENSOR(heater_last_fault_code)
    DEFINE_TSENSOR(heater_dwk)
    DEFINE_BSENSOR(heater_gp_switch)
    DEFINE_BSENSOR(heater_tap_switch)
    DEFINE_BSENSOR(heater_roomtherm)
    DEFINE_BSENSOR(heater_pump_running)
    DEFINE_BSENSOR(heater_alarm_status)
    DEFINE_BSENSOR(heater_cascade_relay)
    DEFINE_BSENSOR(heater_open_therm)
    DEFINE_BSENSOR(heater_gas_valve)
    DEFINE_BSENSOR(heater_spark)
    DEFINE_BSENSOR(heater_ionisation_signal)
    DEFINE_BSENSOR(heater_ot_disabled)
    DEFINE_BSENSOR(heater_has_low_water_pressure)
    DEFINE_BSENSOR(heater_burner_block)
    DEFINE_BSENSOR(heater_gradient_flag)
   protected:
    bool intergas_write_command(const uint8_t *command, uint8_t len);
};

}  // namespace intergas
}  // namespace esphome
