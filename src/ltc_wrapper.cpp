#include "ltc_wrapper.hpp"

#include <LTC6804.h>

#include <LTC6804.cpp>  // used for template functions

// #define DEBUG

static LTC68041 LTC = LTC68041(D8);
unsigned long pec15_error_count = 0;

float raw_voltage_to_real_module_temp(float raw_voltage) {
    return 32.0513f * raw_voltage - 23.0769f;
}

void ltc_init() {
    LTC.initSPI(D7, D6, D5);  // Initialize LTC6804 hardware
}

void ltc_set_balance_bits(std::bitset<12> &balance_bits) {
#ifdef DEBUG
    if (LTC.checkSPI(true)) {
#else
    if (LTC.checkSPI(false)) {
#endif
        digitalWrite(D1, HIGH);
    } else {
        digitalWrite(D1, LOW);
    }

    LTC.cfgSetRefOn(true);
    LTC.cfgSetVUV(3.1);
    LTC.cfgSetVOV(4.2);

    if (balance_bits.any()) {
        digitalWrite(D2, HIGH);
    } else {
        digitalWrite(D2, LOW);
    }

    LTC.cfgSetDCC(balance_bits);
    LTC.cfgWrite();

    // Start different Analog-Digital-Conversions in the Chip
    LTC.startCellConv(LTC68041::DCP_DISABLED);
    delay(5);  // Wait until conversion is finished
    LTC.startAuxConv();
    delay(5);  // Wait until conversion is finished
    LTC.startStatusConv();
    delay(5);  // Wait until conversion is finished
    if (!LTC.cfgRead()) {
        pec15_error_count++;
    }

    // Print the clear text values cellVoltage, gpioVoltage, Undervoltage Bits, Overvoltage Bits
#ifdef DEBUG
    LTC.readCfgDbg();
    LTC.readStatusDbg();
    LTC.readAuxDbg();
    LTC.readCellsDbg();
#endif
}

float ltc_module_temp_1() {
    return raw_voltage_to_real_module_temp(LTC.getAuxVoltage(LTC68041::AuxChannel::CHG_GPIO1));
}

float ltc_module_temp_2() {
    return raw_voltage_to_real_module_temp(LTC.getAuxVoltage(LTC68041::AuxChannel::CHG_GPIO2));
}

float ltc_module_voltage() {
    return LTC.getStatusVoltage(LTC68041::CHST_SOC);
}

float ltc_chip_temp() {
    return LTC.getStatusVoltage(LTC.StatusGroup::CHST_ITMP);
}

std::array<float, 12> ltc_cell_voltages() {
    std::array<float, 12> voltages;
    if (!LTC.getCellVoltages(voltages)) {
        pec15_error_count++;
    }

    return voltages;
}

unsigned long ltc_error_count() {
    return pec15_error_count;
}