#pragma once

#include <array>
#include <bitset>

class BatteryInterface {
   public:
    virtual void init() = 0;
    virtual void set_balance_bits(const std::bitset<12>& balance_bits) = 0;
    virtual float module_temp_1() = 0;
    virtual float module_temp_2() = 0;
    virtual float chip_temp() = 0;
    virtual float module_voltage() = 0;
    virtual std::array<float, 12> cell_voltages() = 0;
    virtual bool balance_error() = 0;
    virtual bool measure_error() = 0;
};