#include <ESP8266httpUpdate.h>

#include <map>

#include "battery_information.hpp"
#include "battery_monitor.hpp"
#include "config.h"
#include "display.hpp"
#include "mqtt_client.hpp"
#include "single_mode_balancer.hpp"
#include "soc.hpp"
#include "timed_history.hpp"
#include "version.h"
#include "wifi.hpp"

#define DEBUG true
#define SSL_ENABLED false

#if DEBUG
#define DEBUG_BEGIN(...) Serial.begin(__VA_ARGS__)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_BEGIN(...)
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

constexpr unsigned long MASTER_TIMEOUT = 5000;
constexpr unsigned long BLINK_TIME = 5000;
constexpr unsigned long LTC_CHECK_INTERVAL = 1000;

String hostname;
String mac_topic;
String module_topic;
bool auto_detect_battery_type;

std::array<unsigned long, 12> cells_to_balance_start{};
std::array<unsigned long, 12> cells_to_balance_interval{};

unsigned long last_ltc_check = 0;
unsigned long last_connection = 0;
unsigned long last_blink_time = 0;
unsigned long long last_master_uptime = 0;

BatteryMonitor battery_monitor(DEBUG);
SingleModeBalancer single_balancer(60 * 1000, 10 * 1000);
MqttClient mqtt(mqtt_server, mqtt_port);
Display display;

bool led_builtin_state = false;

String cpu_description() {
    return String(EspClass::getChipId(), HEX) + " " + EspClass::getCpuFreqMHz() + " MHz";
}

String flash_description() {
    return String(EspClass::getFlashChipId(), HEX) + ", " + (EspClass::getFlashChipSize() / 1024 / 1024) + " of " +
           (EspClass::getFlashChipRealSize() / 1024 / 1024) + " MiB, Mode: " + EspClass::getFlashChipMode() +
           ", Speed: " + (EspClass::getFlashChipSpeed() / 1000 / 1000) + " MHz, Vendor: " + String(EspClass::getFlashChipVendorId(), HEX);
}

void on_mqtt_master_uptime(String, String);
void on_mqtt_balance_request(String, String);
void on_mqtt_read_accurate(String, String);
void on_mqtt_blink(String, String);
void on_mqtt_set_config(String, String);
void on_mqtt_restart(String, String);
void on_mqtt_ota(String, String);

void reconnect() {
    // Loop until we're reconnected
    while (!mqtt.connected()) {
        DEBUG_PRINT("Attempting MQTT connection...");
        if (mqtt.connect()) {
            DEBUG_PRINTLN("connected");
            // Once connected, publish an announcement
            mqtt.publish(module_topic + "/available", "online");
            if (!module_topic.equals(mac_topic)) {
                mqtt.publish(mac_topic + "/available", "undefined");
            }
            mqtt.publish(mac_topic + "/module_topic", module_topic);
            mqtt.publish(mac_topic + "/version", VERSION);
            mqtt.publish(mac_topic + "/build_timestamp", BUILD_TIMESTAMP);
            mqtt.publish(mac_topic + "/wifi", WiFi.SSID());
            mqtt.publish(mac_topic + "/ip", WiFi.localIP().toString());
            mqtt.publish(mac_topic + "/esp_sdk", EspClass::getFullVersion());
            mqtt.publish(mac_topic + "/cpu", cpu_description());
            mqtt.publish(mac_topic + "/flash", flash_description());
            mqtt.publish(mac_topic + "/bms_mode", as_string(bms_mode));
            // Resubscribe
            mqtt.subscribe("master/uptime", on_mqtt_master_uptime);
            for (int i = 0; i < 12; ++i) {
                mqtt.subscribe(module_topic + "/cell/" + (i + 1) + "/balance_request", on_mqtt_balance_request);
            }
            mqtt.subscribe(module_topic + "/read_accurate", on_mqtt_read_accurate);
            mqtt.subscribe(mac_topic + "/blink", on_mqtt_blink);
            mqtt.subscribe(mac_topic + "/set_config", on_mqtt_set_config);
            mqtt.subscribe(mac_topic + "/restart", on_mqtt_restart);
            mqtt.subscribe(mac_topic + "/ota", on_mqtt_ota);
        } else {
            DEBUG_PRINT("failed, rc=");
            DEBUG_PRINT(mqtt.state_string());
            if (last_connection != 0 && millis() - last_connection >= 30000) {
                EspClass::restart();
            }
            delay(1000);
        }
    }
}

String cell_name_from_id(size_t cell_id) {
    size_t cell_number = cell_id + 1;
    if (battery_type == BatteryType::meb8s) {
        if (cell_number <= 4) {
            // do nothing, cell number is correct
        } else if (cell_number <= 8) {
            // These cells dont exist on 8s batteries
            return "undefined";
        } else {
            cell_number = cell_number - 4;
        }
    }

    return String(cell_number);
}

bool is_uint(const String& myString) {
    return std::all_of(myString.begin(), myString.end(), isDigit);
}

int cell_id_from_name(const String& cell_name) {
    // cell number needs to be an unsigned integer
    if (!is_uint(cell_name)) {
        return -1;
    }

    int cell_number = cell_name.toInt();
    int highest_cell_number = 12;
    if (battery_type == BatteryType::meb8s) {
        highest_cell_number = 8;
    }
    // cell number needs to be between 1 and 12 or 1 and 8 for 8s modules
    if (cell_number < 1 || cell_number > highest_cell_number) {
        return -1;
    }

    // do the 8s cell mapping
    if (battery_type == BatteryType::meb8s) {
        if (cell_number >= 5) {
            cell_number = cell_number + 4;
        }
    }

    return cell_number - 1;
}

std::bitset<12> slave_balance_bits() {
    auto balance_bits = std::bitset<12>();
    for (size_t i = 0; i < cells_to_balance_start.size(); ++i) {
        if (millis() - cells_to_balance_start.at(i) <= cells_to_balance_interval.at(i)) {
            balance_bits.set(i, true);
        } else if (cells_to_balance_interval.at(i) > 0) {
            cells_to_balance_interval.at(i) = 0;
        }
    }

    return balance_bits;
}

void publish_mqtt_values(const std::bitset<12>& balance_bits, const String& topic, const BatteryInformation& m) {
    mqtt.publish(module_topic + "/uptime", millis());
    mqtt.publish(module_topic + "/pec15_error_count", battery_monitor.error_count());

    for (size_t i = 0; i < m.cell_voltages.size(); i++) {
        String cell_name = cell_name_from_id(i);
        if (cell_name != "undefined") {
            mqtt.publish(topic + "/cell/" + cell_name + "/voltage", String(m.cell_voltages[i], 3));
            mqtt.publish(module_topic + "/cell/" + cell_name + "/is_balancing", balance_bits.test(i) ? "1" : "0");
        }
    }

    mqtt.publish(topic + "/module_voltage", m.module_voltage);
    mqtt.publish(topic + "/module_temps", String(m.module_temp_1) + "," + String(m.module_temp_2));
    mqtt.publish(topic + "/chip_temp", m.chip_temp);
    mqtt.publish(mac_topic + "/battery_type", as_string(battery_type));
    mqtt.publish(mac_topic + "/auto_detect_battery_type", auto_detect_battery_type);
}

void on_mqtt_master_uptime(String topic_string, String payload_string) {
    DEBUG_PRINT("Got heartbeat from master: ");
    DEBUG_PRINTLN(payload_string);

    digitalWrite(LED_BUILTIN, led_builtin_state);
    led_builtin_state = !led_builtin_state;

    unsigned long long uptime_u_long = std::stoull(payload_string.c_str());

    if (uptime_u_long - last_master_uptime > MASTER_TIMEOUT) {
        DEBUG_PRINTLN(uptime_u_long);
        DEBUG_PRINTLN(last_master_uptime);
        DEBUG_PRINTLN(">>> Master Timeout!!");
    }
    last_master_uptime = uptime_u_long;
}

void on_mqtt_balance_request(String topic_string, String payload_string) {
    String cell_name = topic_string.substring((module_topic + "/cell/").length());
    cell_name = cell_name.substring(0, cell_name.indexOf("/"));
    long cell_id = cell_id_from_name(cell_name);
    if (cell_id == -1) {
        return;
    }
    if (topic_string == module_topic + "/cell/" + cell_name + "/balance_request" && bms_mode == BmsMode::slave) {
        unsigned long balance_time = std::stoul(payload_string.c_str());
        cells_to_balance_start.at(cell_id) = millis();
        cells_to_balance_interval.at(cell_id) = balance_time;
    }
}

void on_mqtt_read_accurate(String topic_string, String payload_string) {
    if (payload_string == "1") {
        last_ltc_check = millis();
        std::bitset<12> balance_bits = slave_balance_bits();
        WiFi.forceSleepBegin();
        delay(1);
        battery_monitor.set_balance_bits(balance_bits);
        WiFi.forceSleepWake();
        delay(1);
        while (WiFi.status() != WL_CONNECTED) {
            delay(100);
        }
        reconnect();
        BatteryInformation bat_info = battery_monitor.info();
        publish_mqtt_values(balance_bits, module_topic + "/accurate", bat_info);
    }
}

void on_mqtt_blink(String topic_string, String payload_string) {
    last_blink_time = millis();
}

void on_mqtt_restart(String topic_string, String payload_string) {
    if (payload_string == "1") {
        EspClass::restart();
    }
}

void on_mqtt_set_config(String topic_string, String payload_string) {
    int indexOfComma = payload_string.indexOf(",");
    String module_number_string;
    if (indexOfComma >= 0) {
        module_number_string = payload_string.substring(0, indexOfComma);
    } else {
        module_number_string = payload_string;
    }

    module_topic = String("esp-module/") + module_number_string;
    mqtt.disconnect();
    reconnect();
}

void on_mqtt_ota(String topic_string, String payload_string) {
    mqtt.publish(mac_topic + "/ota_start", String("ota started [") + payload_string + "] (" + millis() + ")");
    mqtt.publish(mac_topic + "/ota_url", String("https://") + ota_server + payload_string);
    WiFiClientSecure client_secure;
    client_secure.setTrustAnchors(&cert);
    client_secure.setTimeout(60);
    ESPhttpUpdate.setLedPin(LED_BUILTIN, HIGH);
    auto result = ESPhttpUpdate.update(client_secure, String("https://") + ota_server + payload_string);
    String result_string;
    switch (result) {
        case HTTP_UPDATE_FAILED:
            result_string = String("HTTP_UPDATE_FAILED Error (");
            result_string += ESPhttpUpdate.getLastError();
            result_string += "): ";
            result_string += ESPhttpUpdate.getLastErrorString();
            result_string += "\n";
            break;
        case HTTP_UPDATE_NO_UPDATES:
            result_string = "HTTP_UPDATE_NO_UPDATES";
            break;
        case HTTP_UPDATE_OK:
            result_string = "HTTP_UPDATE_OK";
            break;
    }
    DEBUG_PRINTLN(result_string);
    mqtt.publish(mac_topic + "/ota_ret", result_string);
}

// the setup function runs once when you press reset or power the board
[[maybe_unused]] void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D1, OUTPUT);
    digitalWrite(LED_BUILTIN, false);

    DEBUG_BEGIN(74880);
    DEBUG_PRINTLN("init");

    display.init();

    if (use_mqtt) {
        String mac = mac_string();
        hostname = String("easybms-") + mac;
        mac_topic = String("esp-module/") + mac;
        module_topic = mac_topic;

        connect_wifi(hostname, ssid, password);
        digitalWrite(LED_BUILTIN, true);

        mqtt.set_will(module_topic + "/available", 0, true, "offline");
        mqtt.set_id(hostname);
        mqtt.set_user(mqtt_username);
        mqtt.set_password(mqtt_password);
    }

    battery_monitor.init();

    if (battery_type == BatteryType::mebAuto) {
        auto_detect_battery_type = true;
        battery_type = BatteryType::meb12s;
    } else {
        auto_detect_battery_type = false;
    }
}

void update_display(const std::bitset<12>& balance_bits, const BatteryInformation& m) {
    DisplayData data;

    data.measurements = m;
    data.balance_bits = balance_bits;
    data.uptime_seconds = millis() / 1000;

    display.draw(data);
}

// the loop function runs over and over again forever
void loop() {
    if (use_mqtt) {
        // MQTT reconnect if needed
        if (!mqtt.connected()) {
            reconnect();
        }
        last_connection = millis();
        mqtt.loop();
    }

    if (millis() - last_ltc_check > LTC_CHECK_INTERVAL) {
        last_ltc_check = millis();
        BatteryInformation bat_info = battery_monitor.info();

        std::bitset<12> balance_bits{};
        if (bms_mode == BmsMode::slave) {
            balance_bits = slave_balance_bits();
        } else if (bms_mode == BmsMode::single) {
            single_balancer.update_cell_voltages(bat_info.cell_voltages);
            single_balancer.balance();
            balance_bits = single_balancer.balance_bits();
        } else {
            balance_bits = std::bitset<12>(0);
        }
        battery_monitor.set_balance_bits(balance_bits);

        if (use_mqtt) {
            publish_mqtt_values(balance_bits, module_topic, bat_info);
        }

        update_display(balance_bits, bat_info);
    }
    if (millis() - last_blink_time < BLINK_TIME) {
        if ((millis() - last_blink_time) % 100 < 50) {
            digitalWrite(LED_BUILTIN, HIGH);
        } else {
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}
