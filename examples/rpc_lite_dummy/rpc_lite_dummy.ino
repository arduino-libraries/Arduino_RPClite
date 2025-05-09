#include <Arduino_RPClite.h>

DummyTransport transport;
RPCServer server(transport);
RPCClient client(transport);

//int add(int a, int b) { return a + b; }

void setup() {
    //server.bind("add", add);

    // Simulate a call
    float result;
    bool ok = client.call("mult", 2, 3, result);
    server.loop(); // Server processes message
    ok = client.call("mult", 5, 7, result);
}

void loop() {

    Serial.println("looping");

}