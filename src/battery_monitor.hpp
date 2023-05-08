#pragma once

#include <array>
#include <bitset>

#include "battery_type.hpp"
#include "ltc_meb_wrapper.hpp"
#include "timed_history.hpp"

class BatteryMonitor {
   public:
    BatteryMonitor();
    void set_balance_bits(const std::vector<bool>& balance_bits);
    void measure();
    const std::vector<float>& cell_voltages() const;
    const std::vector<bool>& balance_bits() const;
    void set_battery_config(BatteryConfig config);
    BatteryConfig battery_config() const;
    BatteryType battery_type() const;
    float min_voltage() const;
    float max_voltage() const;
    float avg_voltage() const;
    float cell_diff() const;
    float module_voltage() const;
    float module_temp_1() const;
    float module_temp_2() const;
    float chip_temp() const;
    float soc() const;
    uint32_t error_count() const;
    std::optional<float> cell_diff_trend() const;

   private:
    void calc_cell_diff_trend();
    void detect_battery(const std::array<float, 12>& voltages);

    LtcMebWrapper _meb;
    BatteryConfig _battery_config;
    BatteryType _battery_type;

    // Store cell diff history with 1h retention and 1 min granularity
    TimedHistory<float> _cell_diff_history = TimedHistory<float>(1000 * 60 * 60, 1000 * 60);
    std::vector<float> _cell_voltages;
    std::vector<float> _cell_diffs;
    float _min_voltage;
    float _max_voltage;
    float _avg_voltage;
    float _cell_diff;
    float _module_voltage;
    float _module_temp_1;
    float _module_temp_2;
    float _chip_temp;
    float _soc;
    uint32_t _error_count;
    std::optional<float> _cell_diff_trend;
};
