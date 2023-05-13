#include "balancer.hpp"
#include "battery_monitor.hpp"
#include "config.h"
#include "debug.hpp"
#include "display.hpp"
#include "ltc_meb_wrapper.hpp"
#include "mqtt_adapter.hpp"
#include "mqtt_client.hpp"
#include "simulated_battery.hpp"
#include "single_mode_balancer.hpp"
#include "timed_history.hpp"
#include "wifi.hpp"

std::shared_ptr<BatteryMonitor> battery_monitor;
std::shared_ptr<SingleModeBalancer> balancer;
std::shared_ptr<Display> display;
std::shared_ptr<BMS> bms;
std::shared_ptr<MqttAdapter> mqtt_adapter;
std::shared_ptr<BatteryInterface> battery_interface;

[[maybe_unused]] void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D1, OUTPUT);
    digitalWrite(LED_BUILTIN, false);

    DEBUG_BEGIN(74880);
    DEBUG_PRINTLN("init");

    auto mock_battery = std::make_shared<SimulatedBattery>();
    mock_battery->scenario_8s();
    battery_monitor = std::make_shared<BatteryMonitor>(mock_battery);
    battery_monitor->set_battery_config(battery_config);
    balancer = std::make_shared<SingleModeBalancer>(60 * 1000, 10 * 1000);
    display = std::make_shared<Display>();
    bms = std::make_shared<BMS>();
    bms->set_balancer(balancer);
    bms->set_mode(bms_mode);
    bms->set_display(display);
    bms->set_battery_monitor(battery_monitor);

    display->init();

    if (use_mqtt) {
        DEBUG_PRINTLN("Setup MQTT");
        auto hostname = String("easybms-") + mac_string();
        connect_wifi(hostname, ssid, password);
        digitalWrite(LED_BUILTIN, true);
        auto mqtt = std::make_shared<MqttClient>(mqtt_server, mqtt_port);
        mqtt->set_user(mqtt_username);
        mqtt->set_password(mqtt_password);
        mqtt->set_id(hostname);
        mqtt_adapter = std::make_shared<MqttAdapter>(bms, mqtt);
        mqtt_adapter->set_ota_server(ota_server);
        mqtt_adapter->set_ota_cert(&cert);
        mqtt_adapter->init();
        bms->set_mqtt_adapter(mqtt_adapter);
    }
}

void loop() {
    if (mqtt_adapter != nullptr) {
        mqtt_adapter->loop();
    }

    bms->loop();
}
