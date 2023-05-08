#include "wifi.hpp"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <lwip/dns.h>

#include "debug.hpp"

void connect_wifi(String hostname, String ssid, String password) {
    Serial.println();
    Serial.println("connecting to ");
    Serial.println(ssid);
    ESP8266WiFiClass::persistent(false);
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(hostname);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.println(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("DNS1: ");
    Serial.println(IPAddress(dns_getserver(0)));
    Serial.println("DNS2: ");
    Serial.println(IPAddress(dns_getserver(1)));

    randomSeed(micros());
#if SSL_ENABLED
    espClient.setTrustAnchors(&mqtt_cert_store);
#endif
}

String mac_string() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char mac_string[6 * 2 + 1] = {0};
    snprintf(mac_string, 6 * 2 + 1, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(mac_string);
}

String perform_ota_update(String url, const BearSSL::X509List* cert) {
    WiFiClientSecure client_secure;
    client_secure.setTrustAnchors(cert);
    client_secure.setTimeout(60);
    ESPhttpUpdate.setLedPin(LED_BUILTIN, HIGH);
    auto result = ESPhttpUpdate.update(client_secure, String("https://") + url);
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
    return result_string;
}