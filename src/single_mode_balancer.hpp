#pragma once

#include <Arduino.h>

#include <array>
#include <balancer.hpp>
#include <bitset>
#include <cmath>

#include "battery_type.hpp"

class SingleModeBalancer : IBalancer {
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
    std::vector<float> _voltages;
    std::vector<bool> _balance_bits;

    void reset_balance_bits() {
        for (size_t i = 0; i < _balance_bits.size(); i++) {
            _balance_bits[i] = false;
        }
    }

    void select_cells_to_balance() {
        float target = min_voltage();

        if (target <= _cut_off_voltage) {
            // Don't balance
            reset_balance_bits();
        } else {
            // Balance all the cells above target_voltage
            for (size_t i = 0; i < _voltages.size(); i++) {
                if (_voltages[i] > target + 0.003) {
                    _balance_bits[i] = true;
                } else {
                    _balance_bits[i] = false;
                }
            }
        }
    }

    float min_voltage() const {
        float min = _voltages[0];

        for (size_t i = 0; i < _voltages.size(); i++) {
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
    }

    void update_cell_voltages(const std::vector<float>& voltages) {
        _voltages = voltages;
        _voltages_initialized = true;
        _balance_bits.resize(_voltages.size());
    }

    void balance() {
        if (!_voltages_initialized) {
            return;
        }

        long time = millis();

        if (_balancer_state == BalancerState::Relaxing) {
            if (time > _relax_start_timestamp + _relax_time_ms) {
                _balancer_state = BalancerState::Idle;
            }
        }

        if (_balancer_state == BalancerState::Balancing) {
            if (time > _balance_start_timestamp + _balance_time_ms) {
                reset_balance_bits();
                _relax_start_timestamp = time;
                _balancer_state = BalancerState::Relaxing;
            }
        }

        if (_balancer_state == BalancerState::Idle) {
            select_cells_to_balance();
            _balance_start_timestamp = time;
            _balancer_state = BalancerState::Balancing;
        }
    }

    std::vector<bool> balance_bits() const {
        return _balance_bits;
    }
};