/*
    This file is part of the Arduino_RPClite library.

    Copyright (C) Arduino s.r.l. and/or its affiliated companies

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    
*/

#include <Arduino_RPClite.h>

SerialTransport transport(Serial1);
RPCServer server(transport);

float multiply(float a, float b) {
    return a*b;
}

void setup() {
    Serial1.begin(115200);      // eg Nano ESP32 use Serial1.begin(115200, SERIAL_8N1, D4, D5);

    pinMode(LED_BUILTIN, OUTPUT);

    server.bind("multiply", multiply);
}

void blink_before(){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
}

void loop() {
    blink_before();
    server.run();
}