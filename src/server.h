//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_SERVER_H
#define RPCLITE_SERVER_H

#include "rpc.h"
#include "error.h"
#include "wrapper.h"
#include "dispatcher.h"

class RPCServer {
    ITransport& transport;

public:
    RPCServer(ITransport& t) : transport(t) {}

    template<typename F>
    void bind(const MsgPack::str_t& name, F&& func){
        dispatcher.bind(name, func);
    }

    void loop() {

        if (!recv_msg(transport, unpacker)) return;

        int msg_type;
        int msg_id;
        MsgPack::str_t method;
        if (!unpacker.deserialize(msg_type, msg_id, method)){
            Serial.println("unable to deserialize a msg received");
            flush_buffer();
            return;
        }

        MsgPack::arr_size_t resp_size(4);
        packer.clear();
        packer.serialize(resp_size, RESP_MSG, msg_id);

        dispatcher.call(method, unpacker, packer);

        send_msg(transport, packer.packet());

    }

private:

    RpcFunctionDispatcher dispatcher;
    MsgPack::Unpacker unpacker;
    MsgPack::Packer packer;

};

#endif //RPCLITE_SERVER_H
