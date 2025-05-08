#include <Arduino_RPClite.h>
#include <HardwareSerial.h>

HardwareSerial* uart = new HardwareSerial(0);

SerialTransport transport(uart);
RPCServer server(transport);

void setup() {
    transport.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);    
}

void blink_before(){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
}

void loop() {
    blink_before();
    server.loop();
}