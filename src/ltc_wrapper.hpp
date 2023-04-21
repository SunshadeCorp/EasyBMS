#pragma once

#include <bitset>
#include <array>

void ltc_init();
void ltc_set_balance_bits(std::bitset<12> &balance_bits);
float ltc_module_temp_1();
float ltc_module_temp_2();
float ltc_chip_temp();
float ltc_module_voltage();
std::array<float, 12> ltc_cell_voltages();
unsigned long ltc_error_count();