#include "mock_battery.hpp"

MockBattery::MockBattery() {
    // Do nothing
}

void MockBattery::init() {
    // Do nothing
}
void MockBattery::set_balance_bits(const std::bitset<12>& balance_bits) {
    // Do nothing
}

float MockBattery::module_temp_1() {
    return 20;
}
float MockBattery::module_temp_2() {
    return 20;
}
float MockBattery::chip_temp() {
    return 22;
}
float MockBattery::module_voltage() {
    return 3.5 * 12;
}

std::array<float, 12> MockBattery::cell_voltages() {
    auto voltages = std::array<float, 12>();
    voltages[0] = 3.5;
    voltages[1] = 3.5;
    voltages[2] = 3.5;
    voltages[3] = 3.5;
    voltages[4] = 3.5;
    voltages[5] = 3.5;
    voltages[6] = 3.5;
    voltages[7] = 3.5;
    voltages[8] = 3.5;
    voltages[9] = 3.5;
    voltages[10] = 3.5;
    voltages[11] = 3.5;

    return voltages;
}

bool MockBattery::balance_error() {
    return false;
}
bool MockBattery::measure_error() {
    return false;
}