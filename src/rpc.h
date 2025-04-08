//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_RPC_H
#define RPCLITE_RPC_H

#include "transport.h"
#include "MsgPack.h"

inline void send_msg(ITransport& transport, const MsgPack::bin_t<uint8_t>& buffer) {
    size_t size = buffer.size();
    transport.write(reinterpret_cast<const uint8_t*>(&size), sizeof(size)); // Length prefix TODO maybe useless
    transport.write(reinterpret_cast<const uint8_t*>(buffer.data()), size);
}

inline bool recv_msg(ITransport& transport, MsgPack::Unpacker& unpacker) {
    //uint16_t size = 0;
    size_t size = 0;
    if (transport.read(reinterpret_cast<uint8_t*>(&size), sizeof(size)) != sizeof(size)) return false;

    uint8_t* raw_buffer;
    if (transport.read(raw_buffer, size) != size) return false;

    //msgpack::unpack(result, reinterpret_cast<const char*>(buffer.data()), size);
    unpacker.feed(data, size);
    return true;
}

#endif //RPCLITE_RPC_H
