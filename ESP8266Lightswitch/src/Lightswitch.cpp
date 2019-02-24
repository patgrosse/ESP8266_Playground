#include "../include/Lightswitch.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Ticker.h>

#if HTTP

#include <ESP8266HTTPClient.h>

#endif
#if MQTT

#include <AsyncMqttClient.h>

#endif

#include <ESP8266Helper.h>

static int lastGPIOstate;
static bool requestScheduled = false;
static unsigned long scheduledAt;

#if HTTP
static WiFiClient client;
#endif
#if MQTT
static AsyncMqttClient mqttClient;
#endif

static void handleButtonChange() {
    if (requestScheduled) {
        return;
    }
    Serial.println("[INTR] Interrupt scheduled");
    scheduledAt = millis();
    requestScheduled = true;
}

static void checkRequestScheduled() {
    if (requestScheduled && scheduledAt < millis() - 20) {
        int currentGPIOstate = digitalRead(BUTTON_GPIO_PIN);
        if (currentGPIOstate != lastGPIOstate) {
            lastGPIOstate = currentGPIOstate;
            Serial.printf("[LOOP] Button state changed to %d\n", lastGPIOstate);
            Serial.println("[LOOP] Starting request procedure");
            performRequests();
            Serial.println("[LOOP] Finished request procedure");
        }
        requestScheduled = false;
    }
}

static bool ensureOnline() {
    if (!ensureWiFi()) {
        return false;
    }
#if MQTT
    if (!mqttClient.connected()) {
        Serial.println("[SETUP] (Re-)connecting to MQTT broker");
        mqttClient.connect();
        if (!mqttClient.connected()) {
            return false;
        }
    }
#endif
    return true;
}

#if HTTP

static bool performHTTPRequest() {
    HTTPClient http;

    http.begin(client, TOGGLE_URL);

    Serial.println("[HTTP] Performing GET request to " TOGGLE_URL "...\n");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
        Serial.printf("[HTTP] Response code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            Serial.println("[HTTP] Request successful");
            http.end();
            return true;
        }
    } else {
        Serial.printf("[HTTP] Request failed, error: %s\n",
                      HTTPClient::errorToString(httpCode).c_str());
    }

    http.end();
    return false;
}

#endif

bool performRequests() {
    if (ensureOnline()) {
#if MQTT
        Serial.println("[MQTT] Publishing topic " MQTT_TOPIC "...");
        mqttClient.publish(MQTT_TOPIC, 2, false, "1");
#endif
#if HTTP
        Serial.println("[HTTP] Performing HTTP request (try 1/2)");
        if (!performHTTPRequest()) {
            Serial.println("[HTTP] Performing HTTP request (try 2/2)");
            if (!performHTTPRequest()) {
                Serial.println("[HTTP] All HTTP request procedures failed");
                return false;
            }
        }
#endif
    }
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(3000);

    Serial.println("[SETUP] Setting up hardware");
    pinMode(BUTTON_GPIO_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_GPIO_PIN), handleButtonChange,
                    CHANGE);
    lastGPIOstate = digitalRead(BUTTON_GPIO_PIN);
    Serial.printf("[SETUP] Initial button state: %d\n", lastGPIOstate);

    Serial.println("[SETUP] Configuring WiFi");
    setupDHCPWiFi(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("[SETUP] Connecting to WiFi");
    ensureWiFi();

#if MQTT
    Serial.println("[SETUP] Setting up MQTT");
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
#endif

    Serial.println("[SETUP] Setup finished");
    ensureOnline();
}


void loop() {
    delay(10);
    checkRefreshWiFi();
    checkRequestScheduled();
}
