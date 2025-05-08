#include <RPClite.h>

int add(int x, int y) {
    return x + y;
}

void say_hello() {
    Serial.println("Hello!");
}

float divide(float n, float d){
    if (d==0) {
        Serial.println("This would cause an error");
        return 0.0;
    }
    return n/d;
}

int a = 10;
int b = 20;

MsgPack::Packer packer;
MsgPack::Unpacker unpacker;

MsgPack::Packer out_packer;

auto wrapped_add = wrap(add);
auto wrapped_hello = wrap(say_hello);
auto wrapped_divide = wrap(divide);

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

    out_packer.clear();

    blink_before();
    int out = wrapped_add(5, 3);

    bool unpack_ok = wrapped_add(unpacker, out_packer);

    Serial.print("simple call: ");
    Serial.println(out);

    if (unpack_ok){
        Serial.print("unpacker call: ");

        for (size_t i=0; i<out_packer.size(); i++){
            Serial.print(out_packer.data()[i], HEX);
            Serial.print(".");
        }

        Serial.println(" ");
    }
    
    float numerator = 1;
    float denominator = 0;

    packer.clear();
    packer.serialize(numerator, denominator);
    unpacker.clear();
    unpacker.feed(packer.data(), packer.size());
    out_packer.clear();

    bool should_be_false = wrapped_divide(unpacker, out_packer);

    if (should_be_false){
        Serial.print("RPC error call: ");

        for (size_t i=0; i<out_packer.size(); i++){
            Serial.print(out_packer.data()[i], HEX);
            Serial.print(".");
        }

        Serial.println(" ");
    }

    wrapped_hello();
    delay(1000);
}