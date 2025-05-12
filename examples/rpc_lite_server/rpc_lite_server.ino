#include <Arduino_RPClite.h>
#include <HardwareSerial.h>

SerialTransport transport(&Serial2);
RPCServer server(transport);

int add(int a, int b){
    return a+b;
}

MsgPack::str_t greet(){
    return MsgPack::str_t ("Hello Friend");
}

void setup() {
    Serial2.begin(115200);
    transport.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    while(!Serial);

    server.bind("add", add);
    server.bind("greet", greet);

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