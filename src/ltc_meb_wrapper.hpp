#pragma once

#include <LTC6804.h>

#include <array>
#include <bitset>

class LtcMebWrapper {
   public:
    LtcMebWrapper();
    void init();
    void set_balance_bits(const std::bitset<12>& balance_bits);
    float module_temp_1();
    float module_temp_2();
    float chip_temp();
    float module_voltage();
    std::array<float, 12> cell_voltages();
    uint32_t error_count();
    bool balance_error();
    bool measure_error();

   private:
    LTC68041 _ltc;
    uint32_t _pec15_error_count;
    bool _debug_mode;
    bool _balance_error;
    bool _measure_error;
    float raw_voltage_to_real_module_temp(float raw_voltage);
};