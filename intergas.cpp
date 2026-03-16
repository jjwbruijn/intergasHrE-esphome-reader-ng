#include "intergas.h"
#include "esphome/core/log.h"

#include <cinttypes>

namespace esphome {
namespace intergas {

std::string prettify_fault_code(uint8_t code) {
    unsigned u_code = code;
    switch (u_code) {
        case 0:
            return "F000 - Sensor defect";
        case 1:
            return "F001 - Temperature too high during central heating demand";
        case 2:
            return "F002 - Temperature too high during domestic hot water (DHW) demand";
        case 3:
            return "F003 - Flue gas temperature too high";
        case 4:
            return "F004 - No flame during startup";
        case 5:
            return "F005 - Flame disappears during operation";
        case 6:
            return "F006 - Flame simulation error";
        case 7:
            return "F007 - No or insufficient ionisation flow";
        case 8:
            return "F008 - Fan speed incorrect";
        case 9:
            return "F009 - Burner controller has internal fault";
        case 10:
            return "F010 - Sensor fault";
        case 11:
            return "F011 - Sensor fault";
        case 12:
            return "F012 - Sensor 5 fault";
        case 14:
            return "F014 - Mounting fault sensor";
        case 15:
            return "F015 - Mounting fault sensor S1";
        case 16:
            return "F016 - Mounting fault S3";
        case 18:
            return "F018 - Flue and/or air supply duct is blocked";
        case 19:
            return "F019 - BMM error";
        case 27:
            return "F027 - Short circuit of outdoor";
        case 28:
            return "F028 - Reset error";
        case 29:
            return "F029 - Gas valve error";
        case 30:
            return "F030 - Sensor S3 fault";
        case 31:
            return "F031 - Sensor fault S1";
        case 0xff:
            return "No Faults";
        default:
            return "Unspecified fault: " + esphome::to_string(u_code);
    }
}

bool get_bit(uint8_t data, int bit) {
    return data & (1 << bit);
}

float getFloat24(uint8_t msbh, uint8_t msbl, uint8_t lsb) {
    // Calculate float value from four uint8_ts
    uint32_t _msbh = msbh;
    uint32_t _msbl = msbl;
    int32_t dword = (int32_t)(_msbh << 16 | _msbl << 8 | lsb);
    return ((float)dword) / 10000.0;
}

float getFloat32(uint8_t msbh, uint8_t msbl, uint8_t lsbh, uint8_t lsbl) {
    // Calculate float value from four uint8_ts
    uint32_t _msbh = msbh;
    uint32_t _msbl = msbl;
    uint32_t _lsbh = lsbh;
    int32_t dword = (int32_t)(_msbh << 24 | _msbl << 16 | _lsbh << 8 | lsbl);
    return ((float)dword) / 10000.0;
}

float getFloat(uint8_t msb, uint8_t lsb) {
    // Calculate float value from two uint8_ts
    uint16_t _msb = msb;
    int16_t word = (int16_t)(_msb << 8 | lsb);
    return ((float)word) / 100.0;
}

int16_t getInt(uint8_t msb, uint8_t lsb) {
    // Calculate integer value from two uint8_ts
    uint16_t _msb = msb;
    int16_t word = (int16_t)(_msb << 8 | lsb);
    return word;
}

uint16_t getUint(uint8_t msb, uint8_t lsb) {
    // Calculate integer value from two uint8_ts
    uint16_t _msb = msb;
    uint16_t word = (uint16_t)(_msb << 8 | lsb);
    return word;
}

int32_t getInt24(uint8_t msbh, uint8_t msbl, uint8_t lsb) {
    // Calculate 24 bit integer value from two uint8_ts
    uint32_t _msbh = msbh;
    uint32_t _msbl = msbl;
    int32_t word = (int32_t)(_msbh << 16 | _msbl << 8 | lsb);
    return word;
}

int8_t getSigned(uint8_t lsb) {
    // Calculate signed value from one uint8_t
    return (int8_t)lsb;
}

float getTemp(uint8_t msb, uint8_t lsb) {
    uint16_t _msb = msb;
    int16_t word = (int16_t)(_msb << 8 | lsb);
    if ((word <= -5100) || (word == SHRT_MAX)) {
        // Intergas gives -5100 for disconnected sensors
        return nan("1");
    }
    return ((float)word) / 100.0;
}

static const char* const TAG = "intergas";
static const uint8_t INTERGAS_PROCESS[] = {"S?\r"};

void Intergas::setup() {
    return;
}

void Intergas::readData(uint8_t* buffer, uint8_t len) {
    if (len > 64) len = 64;
    this->read_array(buffer, len);
}

#define SEND_COMMAND 0
#define WAIT_RETURN_DATA 1

#define STATE_BLOCK_COUNT 2
#define STATE_BLOCK_STATUS_0 0  // 'stats'
#define STATE_BLOCK_STATUS_1 1  // 'status'
#define STATE_BLOCK_STATUS_2 2  // 'status-extra'
#define STATE_BLOCK_STATUS_3 3  // 'status-extra-2'
#define STATE_BLOCK_STATUS_4 4  // 'params'

#define PUBLISH_SENSOR(member, value_expr)              \
    do {                                                \
        if (this->member##_ != nullptr)                 \
            this->member##_->publish_state(value_expr); \
    } while (0)

bool Intergas::processData(uint8_t type, uint8_t len, uint8_t* sbuf) {
	if(len!=32)return false;
    switch (type) {
        case STATE_BLOCK_STATUS_0:
            PUBLISH_SENSOR(line_power_connected_hours, getUint(sbuf[1], sbuf[0]));
            PUBLISH_SENSOR(line_power_connected_count, getUint(sbuf[3], sbuf[2]));
            PUBLISH_SENSOR(ch_function_hours, getUint(sbuf[5], sbuf[4]));
            PUBLISH_SENSOR(dhw_function_hours, getUint(sbuf[7], sbuf[6]));
            PUBLISH_SENSOR(burner_start_count, getInt24(sbuf[31], sbuf[9], sbuf[8]));
            PUBLISH_SENSOR(ignition_failed, getUint(sbuf[11], sbuf[10]));
            PUBLISH_SENSOR(flame_lost, getUint(sbuf[13], sbuf[12]));
            PUBLISH_SENSOR(reset_count, getUint(sbuf[15], sbuf[14]));
            PUBLISH_SENSOR(gas_meter_ch, getFloat32(sbuf[19], sbuf[18], sbuf[17], sbuf[16]));
            PUBLISH_SENSOR(gas_meter_dhw, getFloat32(sbuf[23], sbuf[22], sbuf[21], sbuf[20]));
            PUBLISH_SENSOR(water_meter, getFloat24(sbuf[28], sbuf[25], sbuf[24]));
            PUBLISH_SENSOR(burner_starts_dhw_count, getInt24(sbuf[29], sbuf[27], sbuf[26]));
            break;
        case STATE_BLOCK_STATUS_1:
            double ch_pressure;
            bool ch_has_pressure_sensor;
            // Temperature sensors inside the Intergas Xtreme.
            PUBLISH_SENSOR(temperature_t1, getTemp(sbuf[1], sbuf[0]));          // s1
            PUBLISH_SENSOR(temperature_t2, getTemp(sbuf[3], sbuf[2]));          // s2
            PUBLISH_SENSOR(temperature_hot_water, getTemp(sbuf[7], sbuf[6]));   // s4
            PUBLISH_SENSOR(temperature_setpoint, getTemp(sbuf[15], sbuf[14]));  // Listed as T.max...
            PUBLISH_SENSOR(fanspeed_set, getInt(sbuf[17], sbuf[16]));
            PUBLISH_SENSOR(fanspeed, getInt(sbuf[19], sbuf[18]));
            ch_pressure = getFloat(sbuf[13], sbuf[12]);
            ch_has_pressure_sensor = get_bit(sbuf[28], 5);
            if (ch_has_pressure_sensor) {
                PUBLISH_SENSOR(pressure, ch_pressure);
            }
            PUBLISH_SENSOR(heater_io_current, getFloat(sbuf[23], sbuf[22]));  // +1
            PUBLISH_SENSOR(heater_pump_running, get_bit(sbuf[26], 3));

            if (get_bit(sbuf[27], 7)) {
                // current listed fault code is the active fault code
                PUBLISH_SENSOR(heater_fault_code, prettify_fault_code(sbuf[29]));
            } else {
                // There is no error active, indicate that.
                PUBLISH_SENSOR(heater_fault_code, prettify_fault_code(0xff));
            }
            // List it at least as an old fault code.
            PUBLISH_SENSOR(heater_last_fault_code, prettify_fault_code(sbuf[29]));

            std::string heater_status;
            unsigned u_heater_status = sbuf[24];
            switch (u_heater_status) {
				case 0:
	                heater_status = "Central Heating";
                    break;			
                case 51:
                    heater_status = "Hot Water After-run";
                    break;
                case 102:
                    heater_status = "Central Heating";
                    break;
                case 126:
                    heater_status = "Standby";
                    break;
                case 170:
                    heater_status = "Service Mode";
                    break;
                case 204:
                    heater_status = "Hot Water";
                    break;
                case 231:
                    heater_status = "Central Heating After-run";
                    break;
                default:
                    heater_status = "Code: " + esphome::to_string(u_heater_status);
                    break;
            }
            PUBLISH_SENSOR(heater_status_code, heater_status);

            PUBLISH_SENSOR(heater_gp_switch, get_bit(sbuf[26], 0));  // Ground pin ?
            PUBLISH_SENSOR(heater_tap_switch, get_bit(sbuf[26], 1));
            PUBLISH_SENSOR(heater_roomtherm, get_bit(sbuf[26], 2));
            PUBLISH_SENSOR(heater_pump_running, get_bit(sbuf[26], 3));
            PUBLISH_SENSOR(heater_dwk, get_bit(sbuf[26], 4) ? "LT Zone" : "HT Zone");
            PUBLISH_SENSOR(heater_alarm_status, get_bit(sbuf[26], 5));
            PUBLISH_SENSOR(heater_cascade_relay, get_bit(sbuf[26], 6));
            PUBLISH_SENSOR(heater_open_therm, get_bit(sbuf[26], 7));

            PUBLISH_SENSOR(heater_gas_valve, get_bit(sbuf[28], 0));
            PUBLISH_SENSOR(heater_spark, get_bit(sbuf[28], 1));
            PUBLISH_SENSOR(heater_ionisation_signal, get_bit(sbuf[28], 2));
            PUBLISH_SENSOR(heater_ot_disabled, get_bit(sbuf[28], 3));
            PUBLISH_SENSOR(heater_has_low_water_pressure, get_bit(sbuf[28], 4));
            PUBLISH_SENSOR(heater_burner_block, get_bit(sbuf[28], 6));
            PUBLISH_SENSOR(heater_gradient_flag, get_bit(sbuf[28], 7));

            break;
    }
    return true;
}

void Intergas::update() {
    static uint8_t updatestep = SEND_COMMAND;
    static uint8_t curCommand = STATE_BLOCK_STATUS_0;
    static uint8_t waitState = 0;
    switch (updatestep) {
        case SEND_COMMAND:
            switch (curCommand) {
                case STATE_BLOCK_STATUS_0:
                    this->intergas_write_command((const uint8_t*)"HN\r", 3);
                    break;
                case STATE_BLOCK_STATUS_1:
                    this->intergas_write_command((const uint8_t*)"S?\r", 3);
                    break;
                case STATE_BLOCK_STATUS_2:
                    this->intergas_write_command((const uint8_t*)"V?\r", 3);
                    break;
                case STATE_BLOCK_STATUS_3:
                    this->intergas_write_command((const uint8_t*)"S2\r", 3);
                    break;
                case STATE_BLOCK_STATUS_4:
                    this->intergas_write_command((const uint8_t*)"V?\r", 3);
                    break;
            }
            updatestep = WAIT_RETURN_DATA;
            break;
        case WAIT_RETURN_DATA:
            uint8_t len = (uint8_t)this->available();
            uint8_t buffer[64];
            if (len) {
                this->readData(buffer, len);
                if (this->processData(curCommand, len, buffer)) {
                    curCommand++;
                    curCommand %= STATE_BLOCK_COUNT;
                }
                waitState = 0;
                updatestep = SEND_COMMAND;
            } else {
                waitState++;
                if (waitState == 2) {
                    ESP_LOGCONFIG(TAG, "Timeout on command %d", curCommand);
                    waitState = 0;
                    updatestep = SEND_COMMAND;
                    curCommand++;
                    curCommand %= STATE_BLOCK_COUNT;
                }
            }
            break;
    }
    return;
}

bool Intergas::intergas_write_command(const uint8_t* command, uint8_t len) {
    while (this->available())
        this->read();
    this->write_array(command, len);
    return true;
}

float Intergas::get_setup_priority() const { return setup_priority::DATA; }

void Intergas::dump_config() {
    ESP_LOGCONFIG(TAG, "Intergas interface loaded");
}

}  // namespace intergas
}  // namespace esphome
