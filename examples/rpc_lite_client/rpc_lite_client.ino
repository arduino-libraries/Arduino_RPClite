#include <RpcLite.h>

SerialTransport transport(&Serial2);
RPCClient client(transport);

void setup() {
    Serial2.begin(115200);
    transport.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    while(!Serial);
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
    
    bool ok = client.call("mult", result, 2.0, 3.0);

    if (ok) {
        Serial.print("Result: ");
        Serial.println(result);
    }

    ok = client.call("divi", result, 2.0, 0.0);

    if (ok) {
        Serial.print("Result: ");
        Serial.println(result);
    }

    delay(2000);
}