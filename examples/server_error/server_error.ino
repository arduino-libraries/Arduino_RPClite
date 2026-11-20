/*
    This file is part of the Arduino_RPClite library.

    Copyright (C) Arduino s.r.l. and/or its affiliated companies

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#include <Arduino_RPClite.h>

#define MY_ERR 0xAA

SerialTransport transport(Serial1);
RPCServer server(transport);

RpcResponseWrapper<float> divide(float a, float b) {
    RpcResponseWrapper<float> response;
    if (b == 0){
        return response.throw_err(MY_ERR, "Cannot divide by zero");
    }
    response.output = a / b;
    return response;
}

void setup() {
    Serial1.begin(115200);
    server.bind("divide", divide);
}

void loop() {
    server.run();
}