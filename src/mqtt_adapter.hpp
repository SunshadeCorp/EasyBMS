#pragma once

#include <BearSSLHelpers.h>
#include <WString.h>

#include <algorithm>
#include <bitset>

#include "bms.hpp"
#include "mqtt_client.hpp"

class BMS;

using time_ms = unsigned long;

class MqttAdapter {
   public:
    MqttAdapter(std::shared_ptr<BMS> bms, std::shared_ptr<MqttClient> mqtt);

    void init();
    void reconnect();
    void loop();
    std::vector<bool> slave_balance_bits();
    void publish();
    void set_ota_server(String ota_server);
    void set_ota_cert(const BearSSL::X509List* cert);

   private:
    std::shared_ptr<BMS> _bms;
    std::shared_ptr<MqttClient> _mqtt;
    static const time_ms MASTER_TIMEOUT = 5000;
    time_ms _last_connection = 0;
    String _hostname;
    String _mac_topic;
    String _module_topic;
    time_ms _last_master_uptime = 0;
    std::vector<time_ms> _balance_start_time{};
    std::vector<time_ms> _balance_duration{};
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
    void reset_balancing(size_t size);
};