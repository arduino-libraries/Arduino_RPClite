#include <Arduino_RPClite.h>

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

MsgPack::Packer packer;
MsgPack::Unpacker unpacker;

void setup() {
    Serial.begin(9600);
}

void loop() {

    blink_before();
    MsgPack::arr_size_t req_sz(4);
    MsgPack::arr_size_t par_sz(2);
    packer.serialize(req_sz, 0, 1, "method", par_sz, 1.0, 2.0);

    DummyTransport dummy_transport(packer.data(), packer.size());
    RpcDecoder<> decoder(dummy_transport);

    while (!decoder.packet_available()){
        Serial.println("Packet not ready");
        decoder.advance();
    }

    Serial.println("packet ready");

}