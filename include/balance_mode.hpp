#pragma once

#include <WString.h>

enum class BalanceMode { slave, single, none };

String as_string(BalanceMode bms_mode);