#pragma once

#include <bitset>
#include <array>
#include <cmath>
#include "battery_type.hpp"

float min(std::array<float, 12> cells, BatteryType battery_type) {
    float min = cells[0];

    for (int i = 0; i < 12; i++) {
        // Disregard the middle 4 values if meb8s battery
        // Those are unused values
        if (battery_type == BatteryType::meb8s && i >= 4 && i < 8) {
            continue;
        }

        if (cells[i] < min) {
            min = cells[i];
        }
    }

    return min;
}

std::bitset<12> balance_algorithm(std::array<float, 12> cell_voltages, BatteryType battery_type) {
    const float cut_off_voltgage = 3.5;
    float min_voltage = min(cell_voltages, battery_type);
    float target_voltage = std::max(min_voltage, cut_off_voltgage);

    std::bitset<12> balance_bits;

    if (min_voltage <= cut_off_voltgage) {
        // Don't balance
        balance_bits = std::bitset<12>(0);
    } else {
        // Balance all the cells above target_voltage
        for(int i = 0; i < 12; i++) {
            if (cell_voltages[i] > target_voltage) {
                balance_bits.set(i);
            }
            else {
                balance_bits.reset(i);
            }
        }
    }

    return balance_bits;
}