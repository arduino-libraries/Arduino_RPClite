#include <RpcLite.h>

SerialTransport transport(&Serial0);
RPCClient client(transport);

void setup() {
    Serial0.begin(115200);
    transport.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
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
    float result;
    blink_before();
    //bool ok = client.call("mult", result, 2.0, 3.0);
    bool ok = client.call("divi", result, 2.0, 0.0);

    if (ok) {
        Serial.print("Result: ");
        Serial.println(result);
    }

    delay(2000);
}