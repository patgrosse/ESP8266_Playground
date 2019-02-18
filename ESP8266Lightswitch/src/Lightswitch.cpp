#include "Lightswitch.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266Helper.h>

int lastGPIOstate;
bool httpScheduled = false;
unsigned long scheduledAt;

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_GPIO_PIN, INPUT_PULLUP);

    setupDHCPWiFi(WIFI_SSID, WIFI_PASSWORD);

    attachInterrupt(digitalPinToInterrupt(BUTTON_GPIO_PIN), handleButtonChange,
                    CHANGE);
    lastGPIOstate = digitalRead(BUTTON_GPIO_PIN);

    Serial.printf("[SETUP] Initial button state: %d\n", lastGPIOstate);
    Serial.println("[SETUP] Setup finished");
}

void handleButtonChange() {
    if (httpScheduled) {
        return;
    }
    Serial.println("[INTR] Interrupt scheduled");
    scheduledAt = millis();
    httpScheduled = true;
}

bool performHTTPRequest() {
    if (ensureWiFi()) {
        HTTPClient http;
        http.begin(TOGGLE_URL);

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
    }
    return false;
}

void loop() {
    delay(10);
    checkRefreshWiFi();
    if (httpScheduled && scheduledAt < millis() - 20) {
        int currentGPIOstate = digitalRead(BUTTON_GPIO_PIN);
        if (currentGPIOstate != lastGPIOstate) {
            lastGPIOstate = currentGPIOstate;
            Serial.printf("[LOOP] Button state changed to %d\n", lastGPIOstate);
            Serial.println("[LOOP] Starting HTTP procedure (try 1/2)");
            if (!performHTTPRequest()) {
                Serial.println("[LOOP] Starting HTTP procedure (try 2/2)");
                if (!performHTTPRequest()) {
                    Serial.println("[LOOP] All HTTP procedures failed");
                }
            }
        }
        httpScheduled = false;
    }
}
