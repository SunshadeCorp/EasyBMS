#include "bms.hpp"

#include "debug.hpp"

void BMS::blink() {
    _last_blink_time = millis();
}

void BMS::flip_led() {
    _led_builtin_state = !_led_builtin_state;
    digitalWrite(LED_BUILTIN, _led_builtin_state ? LOW : HIGH);
}

void BMS::set_led(bool led_state) {
    _led_builtin_state = led_state;
    digitalWrite(LED_BUILTIN, _led_builtin_state ? LOW : HIGH);
}

void BMS::set_module_number(uint8_t module_number) {
    _module_number = module_number;
}

void BMS::restart() {
    ESP.restart();
}

void BMS::set_mode(BalanceMode mode) {
    _mode = mode;
}

BalanceMode BMS::mode() const {
    return _mode;
}

void BMS::set_balancer(std::shared_ptr<IBalancer> balancer) {
    _balancer = balancer;
}

// void BMS::set_display(std::shared_ptr<Display> display) {
//     _display = display;
// }

void BMS::set_mqtt_adapter(std::shared_ptr<MqttAdapter> mqtt_adapter) {
    _mqtt_adapter = mqtt_adapter;
}

void BMS::set_battery_monitor(std::shared_ptr<BatteryMonitor> battery_monitor) {
    _battery_monitor = battery_monitor;
}

std::shared_ptr<const BatteryMonitor> BMS::battery_monitor() {
    return _battery_monitor;
}

void BMS::loop() {
    if (millis() - _last_ltc_check > LTC_CHECK_INTERVAL) {
        _last_ltc_check = millis();
        _battery_monitor->measure();

        if (_balancer != nullptr) {
            _balancer->balance(_battery_monitor->cell_voltages());
            _battery_monitor->set_balance_bits(_balancer->balance_bits());
        }

        if (_mqtt_adapter != nullptr) {
            _mqtt_adapter->reconnect();
            _mqtt_adapter->update();
        }

        // if (_display != nullptr) {
        //     _display->update(_battery_monitor);
        // }
    }

    if (millis() - _last_blink_time < BLINK_TIME) {
        if ((millis() - _last_blink_time) % 100 < 50) {
            set_led(false);
        } else {
            set_led(true);
        }
    }
}