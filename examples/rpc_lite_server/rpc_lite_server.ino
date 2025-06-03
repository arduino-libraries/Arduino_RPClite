#include <Arduino_RPClite.h>

SerialTransport transport(&Serial2);
RPCServer server(transport);

int add(int a, int b){
    return a+b;
}

MsgPack::str_t greet(){
    return MsgPack::str_t ("Hello Friend");
}

MsgPack::str_t loopback(MsgPack::str_t message){
    return message;
}

class multiplier {
public:

    multiplier(){}
    static int mult(int a, int b){
        return a*b;
    }
};


void setup() {
    Serial2.begin(115200);
    transport.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    while(!Serial);

    server.bind("add", add);
    server.bind("greet", greet);
    server.bind("another_greeting", [] {return MsgPack::str_t ("This is a lambda greeting");});
    server.bind("object_multi", &multiplier::mult);

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