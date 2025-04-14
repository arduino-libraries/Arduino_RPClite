#include <RpcLite.h>
#include <HardwareSerial.h>

HardwareSerial* uart = new HardwareSerial(0);

SerialTransport transport(uart);
RPCClient client(transport);

void setup() {
    transport.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    
}

void blink_before(){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
}

void loop() {
    blink_before();
    float result = client.call("mult", 2.0, 3.0);
    delay(1000);
}