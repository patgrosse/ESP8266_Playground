#include "LEDControl.h"

#include <ESP8266WebServer.h>
#include <ESP8266Helper.h>
#include <ESPAsyncUDP.h>

#ifndef HTTP_PORT
#define HTTP_PORT 80
#endif

#ifndef UDP_PORT
#define UDP_PORT 8266
#endif

ESP8266WebServer *webServer;
AsyncUDP *udp;

void initWebServer();

void initUDPServer();

void setup() {
    Serial.begin(115200);
    delay(3000);

    setupDHCPWiFi(WIFI_SSID, WIFI_PASSWORD);
    ensureWiFi();

    initWebServer();
    initUDPServer();

    Serial.println("[SETUP] Setup finished");
}

void loop() {
    webServer->handleClient();
}

void initWebServer() {
    webServer = new ESP8266WebServer(HTTP_PORT);
    webServer->on("/", []() {
        webServer->send(200, "Example request: /red/on");
    });
    webServer->on("/red/on", []() {
        sendHTTPColorChange(Color::red, true);
        webServer->send(200, "text/plain", "ok");
    });
    webServer->on("/red/off", []() {
        sendHTTPColorChange(Color::red, false);
        webServer->send(200, "text/plain", "ok");
    });
    webServer->on("/green/on", []() {
        sendHTTPColorChange(Color::green, true);
        webServer->send(200, "text/plain", "ok");
    });
    webServer->on("/green/off", []() {
        sendHTTPColorChange(Color::green, false);
        webServer->send(200, "text/plain", "ok");
    });
    webServer->on("/blue/on", []() {
        sendHTTPColorChange(Color::blue, true);
        webServer->send(200, "text/plain", "ok");
    });
    webServer->on("/blue/off", []() {
        sendHTTPColorChange(Color::blue, false);
        webServer->send(200, "text/plain", "ok");
    });
    webServer->onNotFound([]() {
        webServer->send(404, "text/plain", "Not found");
    });
    webServer->begin();
    Serial.println("[SETUP] HTTP server started");
}


void handleUDPPacket(AsyncUDPPacket &packet) {
    Serial.printf("[UDP] Received %d bytes from %s, port %d\n",
                  packet.length(),
                  packet.remoteIP().toString().c_str(),
                  packet.remotePort());
    if (packet.length() > 0) {
        String pkt_str((char *) packet.data());
        if (pkt_str.charAt(0) == 'C') {
            Serial.printf("%s\n", pkt_str.c_str());
        } else {
            Serial.printf("[UDP] Unknown packet received: %s\n", pkt_str.c_str());
        }
    }
}

void initUDPServer() {
    udp = new AsyncUDP();
    if (udp->listen(UDP_PORT)) {
        udp->onPacket(handleUDPPacket);
        Serial.println("[SETUP] UDP server started");
    } else {
        Serial.println("[SETUP] Could not start UDP");
    }
}

void sendHTTPColorChange(Color color, bool on) {
    char color_char;
    switch (color) {
        case red:
            color_char = 'R';
            break;
        case green:
            color_char = 'G';
            break;
        case blue:
            color_char = 'B';
            break;
        default:
            return;
    }
    char value = (on ? '1' : '0');
    Serial.printf("[HTTP] Setting color %c to value %c\n", color_char, value);
    Serial.printf("C:%c%c\n", color_char, value);
}
