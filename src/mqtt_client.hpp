#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <wstring.h>

#include <functional>
#include <map>

#define SSL_ENABLED false

using MqttCallback = std::function<void(const String&, const String&)>;

class MqttClient {
   public:
    MqttClient(String server, uint16_t port);

    template <typename T>
    bool publish(String topic, T value);

    bool subscribe(String topic, MqttCallback callback);
    void disconnect();
    bool connected();
    bool connect();
    bool loop();
    void set_password(String password);
    void set_user(String user);
    void set_id(String id);
    void set_will(String topic, uint8_t qos, bool retain, String message);
    String state_string();

   private:
    void pub_sub_client_callback(char* topic, uint8_t* payload, unsigned int length);

#if SSL_ENABLED
    WiFiClientSecure espClient;
#else
    WiFiClient _espClient;
#endif

    PubSubClient _client;
    String _id;
    String _user;
    String _password;
    String _will_topic;
    uint8_t _will_qos;
    bool _will_retain;
    String _will_message;
    bool _use_will;
    std::map<String, MqttCallback> _mqtt_callbacks;
};