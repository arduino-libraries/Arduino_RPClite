//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_CLIENT_H
#define RPCLITE_CLIENT_H
#include "rpc.h"

class RPCClient {
    ITransport& transport;
    uint32_t msg_id = 1;

public:
    RPCClient(ITransport& t) : transport(t) {}

    float call(MsgPack::str_t method, float a, float b) {

        MsgPack::Packer packer;

        uint8_t msg_type = 0;

        packer.serialize(msg_type, msg_id, method, a, b);

        send_msg(transport, packer.packet());

        MsgPack::Unpacker unpacker;
        float result;

        if (recv_msg(transport, unpacker, result)) {
            
            return result;

        }

        return -1; // Error TODO this is clearly wrong. Client should be waiting for response + timeout etc.
    }
};

#endif //RPCLITE_CLIENT_H
