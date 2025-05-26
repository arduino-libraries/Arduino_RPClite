#include <Arduino_RPClite.h>

#ifdef ARDUINO_SAMD_ZERO
#define MSGPACKRPC Serial    // MsgPack RPC runs on the hardware serial port (that do not disconnects on reset/upload)
#define DEBUG      SerialUSB // Debug and upload port is the native USB
#else
#error "Unsupported board"
#endif

SerialTransport transport(&MSGPACKRPC);
RPCClient rpc(transport);

void setup() {
    MSGPACKRPC.begin(115200);
    DEBUG.begin(115200);

    while (!DEBUG) { /* WAIT for serial port to connect */ }

    testSuccessfulCallFl64();
    testWrongCall();
    testSuccessfulCallBool();
    // testWrongCall();
}

void testSuccessfulCallFl64() {
    float result;
    DEBUG.println("mult(2.0, 3.0)");
    bool ok = rpc.call("mult", result, 2.0, 3.0);
    DEBUG.print("-> ");
    if (ok) {
        DEBUG.println(result);
    } else {
        DEBUG.println("error");
    }
}

void testSuccessfulCallBool() {
    bool result;
    DEBUG.println("or(true, false)");
    bool ok = rpc.call("or", result, true, false);
    DEBUG.print("-> ");
    if (ok) {
        DEBUG.println(result ? "true" : "false");
    } else {
        DEBUG.println("error");
    }

    DEBUG.println("or(false)");
    ok = rpc.call("or", result, false);
    DEBUG.print("-> ");
    if (ok) {
        DEBUG.println(result ? "true" : "false");
    } else {
        DEBUG.println("error");
    }
}

void testWrongCall() {
    float result;
    DEBUG.println("mult(2.0)");
    bool ok = rpc.call("mult", result, 2.0);
    DEBUG.print("-> ");
    if (ok) {
        DEBUG.println(result);
    } else {
        DEBUG.println("error");
    }
}

void loop() {
}
