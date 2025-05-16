#define DEBUG
#include <Arduino_RPClite.h>

void blink_before(){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

MsgPack::Packer packer;
MsgPack::Unpacker unpacker;

void setup() {
    Serial.begin(9600);
}

void loop() {

    blink_before();
    MsgPack::arr_size_t req_sz(4);
    MsgPack::arr_size_t notify_sz(3);
    MsgPack::arr_size_t resp_sz(4);
    MsgPack::arr_size_t par_sz(2);

    // REQUEST
    packer.clear();
    packer.serialize(req_sz, 0, 1, "method", par_sz, 1.0, 2.0);

    DummyTransport dummy_transport(packer.data(), packer.size());
    RpcDecoder<> decoder(dummy_transport);

    while (!decoder.packet_incoming()){
        decoder.print_buffer();
        Serial.println("Packet not ready");
        decoder.advance();
        decoder.parse_packet();
        decoder.print_buffer();
        delay(100);
    }

    if (decoder.packet_incoming()){
        Serial.print("packet incoming. type: ");
        Serial.println(decoder.packet_type());
    }

    // NOTIFICATION
    blink_before();
    packer.clear();
    packer.serialize(notify_sz, 2, "method", par_sz, 1.0, 2.0);

    DummyTransport dummy_transport2(packer.data(), packer.size());
    RpcDecoder<> decoder2(dummy_transport2);

    while (!decoder2.packet_incoming()){
        decoder2.print_buffer();
        Serial.println("Packet not ready");
        decoder2.advance();
        decoder2.parse_packet();
        decoder2.print_buffer();
        delay(100);
    }

    if (decoder2.packet_incoming()){
        Serial.print("packet ready. type: ");
        Serial.println(decoder2.packet_type());
    }

    // RESPONSE
    blink_before();
    packer.clear();
    MsgPack::object::nil_t nil;
    MsgPack::arr_size_t ret_sz(2);
    packer.serialize(resp_sz, 1, 1, nil, ret_sz, 3.0, 2);

    DummyTransport dummy_transport3(packer.data(), packer.size());
    RpcDecoder<> decoder3(dummy_transport3);

    while (!decoder3.packet_incoming()){
        decoder3.print_buffer();
        Serial.println("Packet not ready");
        decoder3.advance();
        decoder3.parse_packet();
        decoder3.print_buffer();
        delay(100);
    }

    if (decoder3.packet_incoming()){
        Serial.print("packet ready. type: ");
        Serial.println(decoder3.packet_type());
    }
}