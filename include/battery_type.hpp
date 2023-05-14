#pragma once

#include <WString.h>

#include <array>

enum class BatteryConfig { meb8s, meb12s, mebAuto };
enum class BatteryType { meb8s, meb12s };

BatteryType detect_battery_type(const std::array<float, 12>& voltages);
String as_string(BatteryConfig battery_config);
String as_string(BatteryType battery_type);