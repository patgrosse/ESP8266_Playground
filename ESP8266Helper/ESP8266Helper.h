#ifndef _ESP8266Helper_H_
#define _ESP8266Helper_H_

#include "Arduino.h"

#include <IPAddress.h>

#ifndef WIFI_REFRESH_PERIOD
#define WIFI_REFRESH_PERIOD 1800000
#endif

#ifndef BUTTON_GPIO_PIN
#define BUTTON_GPIO_PIN 2
#endif

void setupDHCPWiFi(String ssid, String passphrase);

void setupStaticWiFi(String ssid, String passphrase, IPAddress ip,
                     IPAddress gateway);

void setupStaticWiFi(String ssid, String passphrase, IPAddress ip,
                     IPAddress gateway, IPAddress subnet);

bool ensureWiFi();

bool checkRefreshWiFi();

#endif
