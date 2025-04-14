//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_RPC_H
#define RPCLITE_RPC_H

#include "transport.h"
#include "MsgPack.h"

inline void send_msg(ITransport& transport, const MsgPack::bin_t<uint8_t>& buffer) {
    size_t size = buffer.size();

    Serial.println("sending data");
    for (size_t i=0; i<size; i++){
        Serial.print(buffer.data()[i]);
        Serial.print(" ");
    }

    transport.write(reinterpret_cast<const uint8_t*>(buffer.data()), size);
}

inline bool recv_msg(ITransport& transport, MsgPack::Unpacker& unpacker, float& result) {

    size_t size = 256;
    uint8_t* raw_buffer = new uint8_t[size];
    if (transport.read(raw_buffer, size) != size) return false;

    //msgpack::unpack(result, reinterpret_cast<const char*>(buffer.data()), size);
    unpacker.feed(raw_buffer, size);
    uint8_t msg_type;
    uint32_t msg_id;
    uint8_t error;

    return unpacker.deserialize(msg_type, msg_id, error, result);
}

#endif //RPCLITE_RPC_H
