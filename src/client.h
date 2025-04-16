//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_CLIENT_H
#define RPCLITE_CLIENT_H
#include "rpc.h"

class RPCClient {
    ITransport& transport;
    int msg_id = 1;

public:
    RPCClient(ITransport& t) : transport(t) {}

    template<typename RType, typename... Args>
    bool call(const MsgPack::str_t method, RType& result, Args&&... args) {

        MsgPack::Packer packer;

        int msg_type = 0;

        MsgPack::arr_size_t call_size(4);

        packer.serialize(call_size, msg_type, msg_id, method);

        MsgPack::arr_size_t arg_size(sizeof...(args));

        packer.serialize(arg_size, std::forward<Args>(args)...);

        send_msg(transport, packer.packet());

        MsgPack::Unpacker unpacker;

        delay(100);

        if (recv_msg(transport, unpacker)) {
            
            int r_msg_type;
            int r_msg_id;
            MsgPack::object::nil_t error;

            MsgPack::arr_size_t resp_size(4);

            bool ok = unpacker.deserialize(resp_size, r_msg_type, r_msg_id, error, result);

            if (!ok){
                //Serial.println("could not serialize resp");
                return false;
            }

            if (r_msg_id != msg_id){
                //Serial.println("msg_id mismatch");
                return false;
            }

            msg_id += 1;
            return true;

        }

        return false;
    }
};

#endif //RPCLITE_CLIENT_H
