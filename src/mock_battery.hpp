#pragma once

#include <array>
#include <bitset>

#include "battery_interface.hpp"

class MockBattery : public BatteryInterface {
   public:
    MockBattery();
    void init() override;
    void set_balance_bits(const std::bitset<12>& balance_bits) override;
    float module_temp_1() override;
    float module_temp_2() override;
    float chip_temp() override;
    float module_voltage() override;
    std::array<float, 12> cell_voltages() override;
    bool balance_error() override;
    bool measure_error() override;
};