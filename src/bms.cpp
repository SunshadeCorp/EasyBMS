#include "bms.hpp"

void BMS::blink() {
    _last_blink_time = millis();
}

void BMS::set_module_number(uint8_t module_number) {
    _module_number = module_number;
}

void BMS::restart() {
    EspClass::restart();
}

void BMS::set_mode(BmsMode mode) {
    _mode = mode;
}

BmsMode BMS::mode() const {
    return _mode;
}

std::shared_ptr<const BatteryMonitor> BMS::battery_monitor() {
    return _battery_monitor;
}

void BMS::loop() {
    if (millis() - _last_ltc_check > LTC_CHECK_INTERVAL) {
        _last_ltc_check = millis();
        _battery_monitor->measure();

        std::vector<bool> balance_bits{};
        if (_mode == BmsMode::slave) {
            balance_bits = _mqtt_adapter->slave_balance_bits();
            _battery_monitor->set_balance_bits(balance_bits);
        } else if (_mode == BmsMode::single) {
            _balancer->update_cell_voltages(_battery_monitor->cell_voltages());
            _balancer->balance();
            balance_bits = _balancer->balance_bits();
            _battery_monitor->set_balance_bits(balance_bits);
        }

        if (_mqtt_adapter != nullptr) {
            _mqtt_adapter->publish_mqtt_values("todo");
        }

        _display->update(_battery_monitor);
    }

    if (millis() - _last_blink_time < BLINK_TIME) {
        if ((millis() - _last_blink_time) % 100 < 50) {
            digitalWrite(LED_BUILTIN, HIGH);
        } else {
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}