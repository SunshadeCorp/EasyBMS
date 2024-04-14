#pragma once

#include "balance_mode.hpp"
#include "balancer_interface.hpp"
#include "battery_monitor.hpp"
#include "display.hpp"
#include "mqtt_adapter.hpp"

class MqttAdapter;

using time_ms = unsigned long;

class BMS {
   public:
    void blink();
    void flip_led();
    void set_led(bool led_state);
    void restart();
    void set_module_number(uint8_t module_number);
    void set_mode(BalanceMode mode);
    void set_balancer(IBalancer* balancer);
    void set_display(Display* display);
    void set_mqtt_adapter(MqttAdapter* mqtt_adapter);
    void set_battery_monitor(BatteryMonitor* battery_monitor);
    BalanceMode mode() const;
    const BatteryMonitor* battery_monitor();
    void loop();

   private:
    time_ms _last_blink_time = 0;
    uint8_t _module_number;
    BalanceMode _mode;
    IBalancer* _balancer;
    MqttAdapter* _mqtt_adapter;
    BatteryMonitor* _battery_monitor;
    Display* _display;
    bool _led_builtin_state = false;
    time_ms _last_ltc_check = 0;
    const time_ms BLINK_TIME = 5000;
    const time_ms LTC_CHECK_INTERVAL = 1000;
};