#include "Arduino.h"
#include "balancer_interface.hpp"
#include "battery_monitor.hpp"
#include "config.h"
#include "debug.hpp"
#include "display.hpp"
#include "ltc_meb_wrapper.hpp"
#include "mock_mqtt_client.hpp"
#include "mqtt_adapter.hpp"
#include "mqtt_client.hpp"
#include "simulated_battery.hpp"
#include "single_mode_balancer.hpp"
#include "timed_history.hpp"
#include "wifi.hpp"

Display display;
LtcMebWrapper meb_battery;
// SimulatedBattery simulated_battery;
BatteryMonitor battery_monitor(&meb_battery);
// BatteryMonitor battery_monitor(&simulated_battery);
BMS bms;
// MockMqttClient mock_mqtt_client;
std::optional<MqttClient> mqtt_client;
std::optional<MqttAdapter> mqtt_adapter;
SingleModeBalancer single_mode_balancer(60 * 1000, 10 * 1000);

[[maybe_unused]] void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D1, OUTPUT);
    digitalWrite(LED_BUILTIN, false);

    DEBUG_BEGIN(74880);
    DEBUG_PRINTLN("init");

    // simulated_battery->scenario_everything_ok();
    battery_monitor.set_battery_config(battery_config);
    bms.set_mode(bms_mode);
    bms.set_display(&display);
    bms.set_battery_monitor(&battery_monitor);

    display.init();

    if (use_mqtt) {
        DEBUG_PRINTLN("Setup MQTT");
        auto hostname = String("easybms-") + mac_string();
        connect_wifi(hostname, ssid, password);
        digitalWrite(LED_BUILTIN, true);

        mqtt_client = MqttClient(mqtt_server, mqtt_port);
        mqtt_client.value().set_user(mqtt_username);
        mqtt_client.value().set_password(mqtt_password);
        mqtt_client.value().set_id(hostname);

        /*
         mock_mqtt_client->is_connected = false;
         mock_mqtt_client->connect_result = true;
         */

        // mqtt_adapter = MqttAdapter(&bms, &mock_mqtt_client);
        mqtt_adapter = MqttAdapter(&bms, &mqtt_client.value());
        mqtt_adapter.value().set_ota_server(ota_server);
        mqtt_adapter.value().set_ota_cert(&cert);
        mqtt_adapter.value().init();
        bms.set_mqtt_adapter(&mqtt_adapter.value());
    }

    if (bms_mode == BalanceMode::single) {
        bms.set_balancer(&single_mode_balancer);
    } else if (bms_mode == BalanceMode::slave && use_mqtt) {
        bms.set_balancer(&mqtt_adapter.value());
    } else if (bms_mode == BalanceMode::none) {
        bms.set_balancer(nullptr);
    }
}

void loop() {
    if (mqtt_adapter.has_value()) {
        mqtt_adapter.value().loop();
    }

    bms.loop();
}
