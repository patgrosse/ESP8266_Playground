#include "UnoLEDControl.h"

#include "Arduino.h"

#define PIN_RED 2
#define PIN_GREEN 3
#define PIN_BLUE 4

// low active
#define LED_ON 0
#define LED_OFF 1

void setRed(bool val) {
    Serial.print("Setting red to ");
    Serial.println(val);
    if (val) {
        digitalWrite(PIN_RED, LED_ON);
    } else {
        digitalWrite(PIN_RED, LED_OFF);
    }
}

void setGreen(bool val) {
    Serial.print("Setting green to ");
    Serial.println(val);
    if (val) {
        digitalWrite(PIN_GREEN, LED_ON);
    } else {
        digitalWrite(PIN_GREEN, LED_OFF);
    }
}

void setBlue(bool val) {
    Serial.print("Setting blue to ");
    Serial.println(val);
    if (val) {
        digitalWrite(PIN_BLUE, LED_ON);
    } else {
        digitalWrite(PIN_BLUE, LED_OFF);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BLUE, OUTPUT);
    setRed(false);
    setGreen(false);
    setBlue(false);
}

void loop() {
    if (Serial.available() > 0) {
        String out = Serial.readStringUntil('\n');
        if (out.startsWith("C:")) {
            bool val = out.charAt(3) == '1';
            switch (out.charAt(2)) {
                case 'R':
                    setRed(val);
                    break;
                case 'G':
                    setGreen(val);
                    break;
                case 'B':
                    setBlue(val);
                    break;
                default:
                    break;
            }
        }
    }
}
