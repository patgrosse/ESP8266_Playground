#include "ESP8266Helper.h"

#include <ESP8266WiFiMulti.h>

const IPAddress subnet_default(255, 255, 255, 0);

ESP8266WiFiMulti WiFiMulti;

void setupCommonWiFi(String ssid, String passphrase) {
    WiFi.mode(WIFI_STA);
    wifi_set_sleep_type(MODEM_SLEEP_T);
    Serial.printf("[WIFI] Will connect to WiFi with SSID %s\n", ssid.c_str());
    WiFiMulti.addAP(ssid.c_str(), passphrase.c_str());
}

void setupDHCPWiFi(String ssid, String passphrase) {
    Serial.println("[WIFI] IP configuration: will fetch from DHCP");
    setupCommonWiFi(ssid, passphrase);
}

void setupStaticWiFi(String ssid, String passphrase, IPAddress ip,
                     IPAddress gateway) {
    setupStaticWiFi(ssid, passphrase, ip, gateway, subnet_default);
}

void setupStaticWiFi(String ssid, String passphrase, IPAddress ip,
                     IPAddress gateway, IPAddress subnet) {
    Serial.printf("[WIFI] IP configuration: IP %s, Gateway %s, Subnet %s\n",
                  ip.toString().c_str(), gateway.toString().c_str(),
                  subnet.toString().c_str());
    WiFi.config(ip, gateway, subnet);
    setupCommonWiFi(ssid, passphrase);
}

bool ensureWiFi() {
    if (!WiFi.isConnected()) {
        Serial.print("[WIFI] Connecting to WiFi");
        Serial.flush();
        while (WiFiMulti.run() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
            Serial.flush();
        }
        Serial.printf("\n[WIFI] WiFi connected with IP ");
        Serial.println(WiFi.localIP());
    }
    return true;
}

bool checkRefreshWiFi() {
    static unsigned long wifi_refresh_on = 0;
    unsigned long current_ms = millis();
    bool ret = true;
    if (wifi_refresh_on <= current_ms) {
        Serial.print("[WIFI] Refreshing WiFi connection...");
        ret = (WiFiMulti.run() == WL_CONNECTED);
        Serial.println(ret ? "success" : "failed");
        wifi_refresh_on = current_ms + WIFI_REFRESH_PERIOD;
    }
    return ret;
}
