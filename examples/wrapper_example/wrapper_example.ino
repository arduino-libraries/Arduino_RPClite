#include <Arduino_RPClite.h>

int add(int x, int y) {
    return x + y;
}

void say_hello() {
    Serial.println("Hello!");
}

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
    blink_before();
    wrapped_add(5, 3);
    wrapped_hello();
    delay(1000);
}