#include "balance_mode.hpp"

String as_string(BalanceMode bms_mode) {
    if (bms_mode == BalanceMode::single) {
        return "single";
    } else if (bms_mode == BalanceMode::slave) {
        return "slave";
    } else if (bms_mode == BalanceMode::none) {
        return "none";
    } else {
        return "invalid";
    }
}