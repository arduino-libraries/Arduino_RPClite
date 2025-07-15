#ifndef RPCLITE_REQUEST_H
#define RPCLITE_REQUEST_H

#define RPC_BUFFER_SIZE  1024


#include "rpclite_utils.h"

class RPCRequest {

public:
    uint8_t buffer[RPC_BUFFER_SIZE];
    size_t size = 0;
    int type = NO_MSG;
    int req_id;
    MsgPack::Packer res_packer;
    MsgPack::Unpacker req_unpacker;
    MsgPack::str_t method;

    bool unpack_request_headers(){
        if (size == 0) return false;

        MsgPack::Unpacker unpacker;

        req_unpacker.clear();
        if (!req_unpacker.feed(buffer, size)) return false;

        int msg_type;
        uint32_t msg_id;
        MsgPack::str_t method;
        MsgPack::arr_size_t req_size;

        if (!req_unpacker.deserialize(req_size, msg_type)) {
            return false; // Header not unpackable
        }

        if (msg_type == CALL_MSG && req_size.size() == REQUEST_SIZE) {
            if (!req_unpacker.deserialize(msg_id, method)) {
                return false; // Method not unpackable
            }
        } else if (msg_type == NOTIFY_MSG && req_size.size() == NOTIFY_SIZE) {
            if (!req_unpacker.deserialize(method)) {
                return false; // Method not unpackable
            }
        } else {
            return false; // Invalid request size/type
        }

        type = msg_type;
        req_id = msg_id;
        return true;

    }

    void pack_response_headers(){
        res_packer.clear();
        MsgPack::arr_size_t resp_size(RESPONSE_SIZE);
        if (msg_type == CALL_MSG) req.res_packer.serialize(resp_size, RESP_MSG, msg_id);
    }

    void reset(){
        size = 0;
        type = NO_MSG;
        method = "";
        req_unpacker.clear();
        res_packer.clear();
    }

};

#endif RPCLITE_REQUEST_H