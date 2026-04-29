/*
    This file is part of the Arduino_RPClite library.

    Copyright (C) Arduino s.r.l. and/or its affiliated companies

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    
*/

#include <Arduino_RPClite.h>

SerialTransport transport(Serial1);
RPCClient client(transport);

void setup() {
    Serial1.begin(115200);      // eg Nano ESP32 use Serial1.begin(115200, SERIAL_8N1, D4, D5);
    Serial1.println("Hello from NANO ESP32");   // this welcome message should be skipped by the server

    pinMode(LED_BUILTIN, OUTPUT);
    delay(10);

    Serial.begin(115200);
}

void blink_before(){
    static bool _blink = true;
    for (int i = 0; i < 6; i++)
    {
        digitalWrite(LED_BUILTIN, _blink);
        delay(200);
        _blink = !_blink;
    }
}

void loop() {
    float result;
    blink_before();

    bool ok = client.call("multiply", result, 2.0, 3.0);

    if (ok) {
        Serial.println("Server returns without issues");
        Serial.print("Result: ");
        Serial.println(result);
    } else
    {
        Serial.println("Server could not handle a call");
    }

}