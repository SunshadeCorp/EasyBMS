#include "battery_monitor.hpp"

#include <LTC6804.cpp>  // used for template functions

// #define DEBUG

BatteryMonitor::BatteryMonitor(bool debug_mode) : _ltc(LTC68041(D8)) {
    _pec15_error_count = 0;
    _debug_mode = debug_mode;
}

void BatteryMonitor::init() {
    _ltc.initSPI(D7, D6, D5);
}

float BatteryMonitor::raw_voltage_to_real_module_temp(float raw_voltage) {
    return 32.0513f * raw_voltage - 23.0769f;
}

void BatteryMonitor::set_balance_bits(const std::bitset<12> &balance_bits) {
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
    if (!_ltc.getCellVoltages<12>(voltages)) {
        _pec15_error_count++;
        Serial.println("error");
    }

    for (int i = 0; i < 12; i++) {
        Serial.println(voltages[i]);
    }
    Serial.println();

    return voltages;
}

unsigned long BatteryMonitor::error_count() {
    return _pec15_error_count;
}