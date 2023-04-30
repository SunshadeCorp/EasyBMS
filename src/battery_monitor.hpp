#pragma once

#include <LTC6804.h>

#include <array>
#include <bitset>

class BatteryMonitor {
   public:
    BatteryMonitor(bool debug_mode);
    void init();
    void set_balance_bits(const std::bitset<12>& balance_bits);
    float module_temp_1();
    float module_temp_2();
    float chip_temp();
    float module_voltage();
    std::array<float, 12> cell_voltages();
    unsigned long error_count();

   private:
    LTC68041 _ltc;
    unsigned long _pec15_error_count;
    bool _debug_mode;

    float raw_voltage_to_real_module_temp(float raw_voltage);
};
