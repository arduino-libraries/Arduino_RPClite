//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_CLIENT_H
#define RPCLITE_CLIENT_H
#include "rpc.h"
#include "error.h"

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

        // blocking call
        while (!recv_msg(transport, unpacker)){delay(1);}

        int r_msg_type;
        int r_msg_id;
        MsgPack::object::nil_t nil;
        RpcError rpc_error;

        MsgPack::arr_size_t resp_size(4);

        if (!unpacker.deserialize(resp_size, r_msg_type, r_msg_id, nil, result)){
            //Try to deserialize for a RpcError
            if (!unpacker.deserialize(resp_size, r_msg_type, r_msg_id, rpc_error, nil)){
                Serial.println("Unable to deserialize");
                Serial.print(raw_buffer_fill);
                for (size_t i = 0; i<raw_buffer_fill; i++){
                    Serial.print(raw_buffer[i], HEX);
                    Serial.print("-");
                }
                flush_buffer();
                return false;
            } else {
                Serial.print("RPC produced an error: ");
                Serial.println(rpc_error.code);
                Serial.println(rpc_error.traceback);
            }

        }

        if (r_msg_id != msg_id){
            //Serial.println("msg_id mismatch");
            flush_buffer();
            return false;
        }

        msg_id += 1;

        flush_buffer();
        return true;

    }
};

#endif //RPCLITE_CLIENT_H
