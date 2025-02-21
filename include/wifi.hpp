#pragma once

#include <WString.h>

void connect_wifi(String hostname, String ssid, String password);

String mac_string();

String perform_ota_update(String url, const char* cert);