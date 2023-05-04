#include "battery_monitor.hpp"

#include <LTC6804.cpp>  // used for template functions

#include "soc.hpp"

// #define DEBUG

BatteryMonitor::BatteryMonitor(bool debug_mode) : _ltc(LTC68041(D8)) {
    _pec15_error_count = 0;
    _debug_mode = debug_mode;
    _measure_error = false;
    _balance_error = false;
}

void BatteryMonitor::init() {
    _ltc.initSPI(D7, D6, D5);
}

float BatteryMonitor::raw_voltage_to_real_module_temp(float raw_voltage) {
    return 32.0513f * raw_voltage - 23.0769f;
}

void BatteryMonitor::set_balance_bits(const std::bitset<12> &balance_bits) {
    _balance_error = false;

    if (_ltc.checkSPI(_debug_mode)) {
        digitalWrite(D1, HIGH);
    } else {
        digitalWrite(D1, LOW);
    }

    _ltc.cfgSetRefOn(true);
    _ltc.cfgSetVUV(3.1);
    _ltc.cfgSetVOV(4.2);

    if (balance_bits.any()) {
        digitalWrite(D2, HIGH);
    } else {
        digitalWrite(D2, LOW);
    }

    _ltc.cfgSetDCC(balance_bits);
    _ltc.cfgWrite();

    // Start different Analog-Digital-Conversions in the Chip
    _ltc.startCellConv(LTC68041::DCP_DISABLED);
    delay(5);  // Wait until conversion is finished
    _ltc.startAuxConv();
    delay(5);  // Wait until conversion is finished
    _ltc.startStatusConv();
    delay(5);  // Wait until conversion is finished
    if (!_ltc.cfgRead()) {
        _pec15_error_count++;
        _balance_error = true;
    }

    // Print the clear text values cellVoltage, gpioVoltage, Undervoltage Bits, Overvoltage Bits
    if (_debug_mode) {
        _ltc.readCfgDbg();
        _ltc.readStatusDbg();
        _ltc.readAuxDbg();
        _ltc.readCellsDbg();
    }
}

float BatteryMonitor::module_temp_1() {
    return raw_voltage_to_real_module_temp(_ltc.getAuxVoltage(LTC68041::AuxChannel::CHG_GPIO1));
}

float BatteryMonitor::module_temp_2() {
    return raw_voltage_to_real_module_temp(_ltc.getAuxVoltage(LTC68041::AuxChannel::CHG_GPIO2));
}

float BatteryMonitor::module_voltage() {
    return _ltc.getStatusVoltage(LTC68041::CHST_SOC);
}

float BatteryMonitor::chip_temp() {
    return _ltc.getStatusVoltage(_ltc.StatusGroup::CHST_ITMP);
}

std::array<float, 12> BatteryMonitor::cell_voltages() {
    std::array<float, 12> voltages;
    bool success = _ltc.getCellVoltages<12>(voltages);
    if (success) {
        _measure_error = false;
    } else {
        _pec15_error_count++;
        _measure_error = true;
    }

    return voltages;
}

unsigned long BatteryMonitor::error_count() {
    return _pec15_error_count;
}

bool BatteryMonitor::measure_error() {
    return _measure_error;
}

bool BatteryMonitor::balance_error() {
    return _balance_error;
}

BatteryInformation BatteryMonitor::info() {
    std::array<float, 12> voltages = cell_voltages();

    if (auto_detect_battery_type) {
        battery_type = detect_battery_type(cell_voltages);
    }

    float sum = 0.0f;
    float voltage_min = voltages[0];
    float voltage_max = voltages[0];
    for (size_t i = 0; i < voltages.size(); i++) {
        if (battery_type == BatteryType::meb8s && i >= 4 && i < 8) {
            continue;
        }

        sum += voltages[i];
        if (voltages[i] < voltage_min) {
            voltage_min = voltages[i];
        }
        if (voltages[i] > voltage_max) {
            voltage_max = voltages[i];
        }
    }
    float voltage_avg = 0;
    if (battery_type == BatteryType::meb8s) {
        voltage_avg = sum / 8.0f;
    } else {
        voltage_avg = sum / 12.0f;
    }

    BatteryInformation m;
    for (size_t i = 0; i < voltages.size(); i++) {
        m.cell_diffs_to_avg[i] = voltages[i] - voltage_avg;
    }
    m.cell_voltages = voltages;
    m.module_voltage = module_voltage();
    m.module_temp_1 = module_temp_1();
    m.module_temp_2 = module_temp_2();
    m.chip_temp = chip_temp();
    m.avg_cell_voltage = voltage_avg;
    m.min_cell_voltage = voltage_min;
    m.max_cell_voltage = voltage_max;
    m.cell_diff = voltage_max - voltage_min;
    m.soc = voltage_to_soc(voltage_avg);
    m.cell_diff_trend = 0.0f;

    // Calculate cell diff trend
    _cell_diff_history.insert(m.cell_diff);
    long current_time = millis();
    auto result = _cell_diff_history.avg_element();
    if (result.has_value()) {
        float history_cell_diff = result.value().value;
        float history_timestamp = result.value().timestamp;

        if (current_time > history_timestamp) {
            // Cell diff change per hour in the last hour
            float change = m.cell_diff - history_cell_diff;
            float time_hours = (float)(current_time - history_timestamp) / (float)(1000 * 60 * 60);
            m.cell_diff_trend = change / time_hours;
        }
    }

    return m;
}