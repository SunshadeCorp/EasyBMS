#pragma once

#include <Esp.h>
#include <WCharacter.h>
#include <WString.h>

#include <algorithm>
#include <bitset>

#include "bms.hpp"
#include "debug.hpp"
#include "mqtt_client.hpp"
#include "version.h"
#include "wifi.hpp"

class BMS;

class MqttAdapter {
   public:
    MqttAdapter(std::shared_ptr<BMS> bms, std::shared_ptr<MqttClient> mqtt);

    void init();
    void reconnect();
    void loop();
    std::vector<bool> slave_balance_bits();
    void publish_mqtt_values(const String& topic);
    void set_ota_server(String ota_server);
    void set_ota_cert(const BearSSL::X509List* cert);

   private:
    std::shared_ptr<BMS> _bms;
    std::shared_ptr<MqttClient> _mqtt;
    static const unsigned long MASTER_TIMEOUT = 5000;
    unsigned long _last_connection = 0;
    String _hostname;
    String _mac_topic;
    String _module_topic;
    unsigned long long _last_master_uptime = 0;
    std::array<unsigned long, 12> _cells_to_balance_start{};
    std::array<unsigned long, 12> _cells_to_balance_interval{};
    const BearSSL::X509List* _ota_cert;
    String _ota_server;

    void on_mqtt_master_uptime(String topic_string, String payload_string);
    void on_mqtt_balance_request(String topic_string, String payload_string);
    void on_mqtt_read_accurate(String topic_string, String payload_string);
    void on_mqtt_blink(String topic_string, String payload_string);
    void on_mqtt_restart(String topic_string, String payload_string);
    void on_mqtt_set_config(String topic_string, String payload_string);
    void on_mqtt_ota(String topic_string, String payload_string);
    bool is_uint(const String& number_string) const;
    int cell_id_from_name(const String& cell_name) const;
    String cpu_description() const;
    String flash_description() const;
};