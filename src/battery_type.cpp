#include "battery_type.hpp"

BatteryType detect_battery_type(const std::array<float, 12>& voltages) {
    if (voltages[0] >= 0.01f && voltages[1] >= 0.01f && voltages[2] >= 0.01f && voltages[3] >= 0.01f && voltages[4] < 0.01f && voltages[5] < 0.01f &&
        voltages[6] < 0.01f && voltages[7] < 0.01f && voltages[8] >= 0.01f && voltages[9] >= 0.01f && voltages[10] >= 0.01f && voltages[11] >= 0.01f) {
        return BatteryType::meb8s;
    } else {
        return BatteryType::meb12s;
    }
}

String as_string(BatteryConfig battery_config) {
    if (battery_config == BatteryConfig::meb12s) {
        return "meb12s";
    } else if (battery_config == BatteryConfig::meb8s) {
        return "meb8s";
    } else if (battery_config == BatteryConfig::mebAuto) {
        return "mebAuto";
    } else {
        return "invalid";
    }
}

String as_string(BatteryType battery_type) {
    if (battery_type == BatteryType::meb12s) {
        return "meb12s";
    } else if (battery_type == BatteryType::meb8s) {
        return "meb8s";
    } else {
        return "invalid";
    }
}