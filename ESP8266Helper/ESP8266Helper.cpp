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
    Serial.print("[WIFI] Connecting to WiFi");
    Serial.flush();
    int counter = 0;
    while (WiFiMulti.run() != WL_CONNECTED) {
        if (counter > 8) {
            Serial.println(
                    "[WIFI] Could not connect to WiFi in 4 seconds! Aborting");
            return false;
        }
        delay(500);
        Serial.print(".");
        Serial.flush();
        counter++;
    }
    Serial.printf("\n[WIFI] WiFi connected with IP ");
    Serial.println(WiFi.localIP());
    return true;
}

bool checkRefreshWiFi() {
    static unsigned long wifi_refresh_on = 0;
    unsigned long current_ms = millis();
    bool ret = true;
    if (wifi_refresh_on <= current_ms) {
        Serial.println("[WIFI] Refreshing WiFi connection...");
        ret = ensureWiFi();
        wifi_refresh_on = current_ms + WIFI_REFRESH_PERIOD;
    }
    return ret;
}
