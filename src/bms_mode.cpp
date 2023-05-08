#include "bms_mode.hpp"

String as_string(BmsMode bms_mode) {
    if (bms_mode == BmsMode::single) {
        return "single";
    } else if (bms_mode == BmsMode::slave) {
        return "slave";
    } else {
        return "invalid";
    }
}