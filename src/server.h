//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_SERVER_H
#define RPCLITE_SERVER_H

#include "rpc.h"
#include "error.h"

class RPCServer {
    ITransport& transport;

public:
    RPCServer(ITransport& t) : transport(t) {}

    int response_type = 1;

    void loop() {
        MsgPack::Unpacker unpacker;
        if (!recv_msg(transport, unpacker)) return;

        int msg_type;
        int msg_id;
        MsgPack::str_t method;
        float a;
        float b;

        unpacker.deserialize(msg_type, msg_id, method, a, b);

        float result = a * b;

        MsgPack::Packer packer;

        MsgPack::object::nil_t null;

        packer.serialize(response_type, msg_id, null, result);
        send_msg(transport, packer.packet());

    }
};

#endif //RPCLITE_SERVER_H
