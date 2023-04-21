#pragma once

#include <bitset>
#include <array>
#include <cmath>
#include "battery_type.hpp"

#include <Arduino.h>

class SingleModeBalancer {
private:
    enum class BalancerState {
        Idle,
        Balancing,
        Relaxing,
    };

    long _balance_time_ms;
    long _relax_time_ms;

    long _balance_start_timestamp;
    long _relax_start_timestamp;

    float _cut_off_voltage;

    BalancerState _balancer_state;
    bool _voltages_initialized;
    std::array<float, 12> _voltages;
    std::bitset<12> _balance_bits;

    void balance_algorithm() {
        float target = min_voltage();

        if (target <= _cut_off_voltage) {
            // Don't balance
            _balance_bits.reset();
        } else {
            // Balance all the cells above target_voltage
            for(int i = 0; i < 12; i++) {
                if (_voltages[i] > target + 0.003) {
                    _balance_bits.set(i);
                }
                else {
                    _balance_bits.reset(i);
                }
            }
        }
    }

    float min_voltage() {
        float min = _voltages[0];
        BatteryType bat_type = detect_battery_type(_voltages);

        for (int i = 0; i < 12; i++) {
            if (bat_type == BatteryType::meb8s && i >= 4 && i < 8) {
                continue;
            }

            if (_voltages[i] < min) {
                min = _voltages[i];
            }
        }

        return min;
    }

public:
    SingleModeBalancer(long balance_time_ms, long relax_time_ms) {
        _balance_time_ms = balance_time_ms;
        _relax_time_ms = relax_time_ms;
        _balance_start_timestamp = 0;
        _relax_start_timestamp = 0;
        _cut_off_voltage = 3.5;
        _balancer_state = BalancerState::Idle;
        _voltages_initialized = false;
        _voltages.fill(0);
        _balance_bits.reset();
    }

    void update_cell_voltages(const std::array<float, 12>& voltages) {
        _voltages = voltages;
        _voltages_initialized = true;
    }

    void balance() {
        long time = millis();

        if (!_voltages_initialized) {
            return;
        }

        if (_balancer_state == BalancerState::Relaxing) {
            if (time > _relax_start_timestamp + _relax_time_ms) {
                _balancer_state = BalancerState::Idle;
            }
        }

        if (_balancer_state == BalancerState::Balancing) {
            if (time > _balance_start_timestamp + _balance_time_ms) {
                _balance_bits.reset();
                _relax_start_timestamp = time;
                _balancer_state = BalancerState::Relaxing;
            }
        }

        if (_balancer_state == BalancerState::Idle) {
            balance_algorithm();
            _balance_start_timestamp = time;
            _balancer_state = BalancerState::Balancing;
        }
    }

    std::bitset<12> balance_bits() {
        return _balance_bits;
    }

};