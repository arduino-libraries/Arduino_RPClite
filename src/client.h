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

        while (true){
            if (!recv_msg(transport, unpacker)){
                delay(1);
                continue;
            }

            int r_msg_type;
            int r_msg_id;
            MsgPack::object::nil_t nil;
            RpcError rpc_error;

            MsgPack::arr_size_t resp_size;

            if (!unpacker.deserialize(resp_size, r_msg_type, r_msg_id)){
                Serial.println("malformed response");
            };

            if ((resp_size.size() != 4) || (r_msg_type != 1) || (r_msg_id != msg_id)){
                Serial.println("wrong msg received");
                flush_buffer();
                return false;
            }

            if (!unpacker.unpackable(nil)){
                Serial.print("RPC error - ");
                if (!unpacker.deserialize(rpc_error, nil)){
                    Serial.println("wrong error msg received");
                    continue;
                }
                Serial.print(" error code: ");
                Serial.print(rpc_error.code);
                Serial.print(" error str: ");
                Serial.println(rpc_error.traceback);
                msg_id += 1;
                flush_buffer();
                return false;
            } else if (!unpacker.deserialize(nil, result)){
                Serial.println("Unexpected result");
                continue;
            }
            break;
        }

        msg_id += 1;

        flush_buffer();
        return true;

    }
};

#endif //RPCLITE_CLIENT_H
