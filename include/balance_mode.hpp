#pragma once

#include <WString.h>

enum class BalanceMode { slave, single };

String as_string(BalanceMode bms_mode);