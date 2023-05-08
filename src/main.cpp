#include <map>

#include "balancer.hpp"
#include "battery_monitor.hpp"
#include "config.h"
#include "debug.hpp"
#include "display.hpp"
#include "mqtt_adapter.hpp"
#include "mqtt_client.hpp"
#include "single_mode_balancer.hpp"
#include "timed_history.hpp"
#include "wifi.hpp"

std::shared_ptr<BatteryMonitor> battery_monitor;
std::shared_ptr<SingleModeBalancer> balancer;
std::shared_ptr<Display> display;
std::shared_ptr<BMS> bms;
std::shared_ptr<MqttAdapter> mqtt_adapter;

[[maybe_unused]] void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D1, OUTPUT);
    digitalWrite(LED_BUILTIN, false);

    DEBUG_BEGIN(74880);
    DEBUG_PRINTLN("init");

    battery_monitor = std::make_shared<BatteryMonitor>();
    balancer = std::make_shared<SingleModeBalancer>(60 * 1000, 10 * 1000);
    display = std::make_shared<Display>();
    bms = std::make_shared<BMS>();

    display->init();
    bms->set_mode(bms_mode);

    auto hostname = String("easybms-") + mac_string();
    connect_wifi(hostname, ssid, password);
    digitalWrite(LED_BUILTIN, true);

    if (use_mqtt) {
        auto mqtt = std::make_shared<MqttClient>(mqtt_server, mqtt_port);
        mqtt->set_user(mqtt_username);
        mqtt->set_password(mqtt_password);
        mqtt->set_id(hostname);
        mqtt_adapter = std::make_shared<MqttAdapter>(bms, mqtt);
        mqtt_adapter->set_ota_server(ota_server);
        mqtt_adapter->set_ota_cert(&cert);
        mqtt_adapter->init();
    }

    battery_monitor->set_battery_config(battery_config);
}

// the loop function runs over and over again forever
void loop() {
    if (use_mqtt) {
        mqtt_adapter->loop();
    }

    bms->loop();
}
