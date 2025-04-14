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
    Serial.println("");

    transport.write(reinterpret_cast<const uint8_t*>(buffer.data()), size);
}

inline bool recv_msg(ITransport& transport, MsgPack::Unpacker& unpacker) {

    size_t size = 256;
    uint8_t raw_buffer[size] = {0};

    int attempts = 0;

    size_t bytes_read = 0;

    while ((attempts<100) && (bytes_read == 0)) {
        bytes_read = transport.read(raw_buffer, size);
        attempts++;
        delay(10);
    }

    if (bytes_read == 0){
        Serial.println("no bytes received");
        return false;
    }

    unpacker.feed(raw_buffer, bytes_read);

    Serial.println("got data");
    for (size_t i=0; i<bytes_read; i++){
        Serial.print(raw_buffer[i]);
        Serial.print(" ");
    }
    Serial.println("");

    return true;
}

#endif //RPCLITE_RPC_H
