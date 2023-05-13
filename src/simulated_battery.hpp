#pragma once

#include <array>
#include <bitset>

#include "battery_interface.hpp"

class SimulatedBattery : public BatteryInterface {
   public:
    SimulatedBattery();
    void init() override;
    void set_balance_bits(const std::bitset<12>& balance_bits) override;
    float module_temp_1() override;
    float module_temp_2() override;
    float chip_temp() override;
    float module_voltage() override;
    std::array<float, 12> cell_voltages() override;
    bool balance_error() override;
    bool measure_error() override;

    void scenario_everything_ok();
    void scenario_balance();
    void scenario_measure_error();
    void scenario_8s();

   private:
    std::array<float, 12> _voltages;
    std::bitset<12> _balance_bits;
    bool _measure_error = false;
    bool _balance_error = false;
};