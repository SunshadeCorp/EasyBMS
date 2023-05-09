#pragma once

#include <balancer.hpp>
#include <vector>

class SingleModeBalancer : IBalancer {
   public:
    SingleModeBalancer(long balance_time_ms, long relax_time_ms);
    void update_cell_voltages(const std::vector<float>& voltages);
    void balance();
    std::vector<bool> balance_bits() const;

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

    void reset_balance_bits();
    void select_cells_to_balance();
    float min_voltage() const;
};