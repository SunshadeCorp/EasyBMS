#pragma once

#include <WString.h>

enum class BmsMode { slave, single };

String as_string(BmsMode bms_mode);