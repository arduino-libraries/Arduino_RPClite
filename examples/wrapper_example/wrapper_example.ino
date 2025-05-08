#include <RPClite.h>

int add(int x, int y) {
    return x + y;
}

void say_hello() {
    Serial.println("Hello!");
}

int a = 10;
int b = 20;

MsgPack::Packer packer;
MsgPack::Unpacker unpacker;

auto wrapped_add = wrap(add);
auto wrapped_hello = wrap(say_hello);

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

void setup() {
    Serial.begin(9600);
}

void loop() {

    packer.clear();
    packer.serialize(a, b);

    unpacker.clear();
    unpacker.feed(packer.data(), packer.size());

    blink_before();
    int out = wrapped_add(5, 3);
    int out_unpack = wrapped_add(unpacker);

    Serial.print("simple call: ");
    Serial.println(out);
    Serial.print("unpacker call: ");
    Serial.println(out_unpack);
    
    wrapped_hello();
    delay(1000);
}