#include <RpcLite.h>

DummyTransport transport;
RPCServer server(transport);
RPCClient client(transport);

//int add(int a, int b) { return a + b; }

void setup() {
    //server.bind("add", add);

    // Simulate a call
    int result = client.call("mult", 2, 3);
    server.loop(); // Server processes message
    result = client.call("mult", 5, 7);
}

void loop() {

    Serial.println("looping");

}