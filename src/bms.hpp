#pragma once

#include "balancer.hpp"
#include "battery_monitor.hpp"
#include "bms_mode.hpp"
#include "display.hpp"
#include "mqtt_adapter.hpp"
#include "single_mode_balancer.hpp"

class MqttAdapter;

class BMS {
   public:
    void blink();
    void flip_led();
    void set_led(bool led_state);
    void restart();
    void set_module_number(uint8_t module_number);
    void set_mode(BmsMode mode);
    BmsMode mode() const;
    std::shared_ptr<const BatteryMonitor> battery_monitor();
    void loop();

   private:
    unsigned long _last_blink_time = 0;
    uint8_t _module_number;
    BmsMode _mode;
    std::shared_ptr<SingleModeBalancer> _balancer;
    std::shared_ptr<MqttAdapter> _mqtt_adapter;
    std::shared_ptr<BatteryMonitor> _battery_monitor;
    std::shared_ptr<Display> _display;
    bool _led_builtin_state = false;
    unsigned long _last_ltc_check = 0;
    const unsigned long BLINK_TIME = 5000;
    const unsigned long LTC_CHECK_INTERVAL = 1000;
};