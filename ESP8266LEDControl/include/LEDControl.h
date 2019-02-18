#ifndef _ESP8266Server_H_
#define _ESP8266Server_H_

#include "Arduino.h"

enum Color {
    red, green, blue
};

void sendHTTPColorChange(Color color, bool on);

#endif /* _ESP8266Server_H_ */
