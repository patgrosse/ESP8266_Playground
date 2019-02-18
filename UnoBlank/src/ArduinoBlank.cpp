#include "ArduinoBlank.h"

#include "Arduino.h"
#include "limits.h"

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
    delay(ULONG_MAX);
}
