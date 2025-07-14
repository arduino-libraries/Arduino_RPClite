#ifndef RPCLITE_SERVER_H
#define RPCLITE_SERVER_H

#include "request.h"
#include "error.h"
#include "wrapper.h"
#include "dispatcher.h"
#include "decoder.h"
#include "decoder_manager.h"
#include "SerialTransport.h"

#define MAX_CALLBACKS   100

class RPCServer {

public:
    RPCServer(ITransport& t) : decoder(&RpcDecoderManager<>::getDecoder(t)) {}

    // This constructor was removed because it leads to decoder duplication
    // RPCServer(Stream& stream) {
    //     ITransport* transport = (ITransport*) new SerialTransport(stream);
    //     decoder = &RpcDecoderManager<>::getDecoder(*transport);
    // }

    template<typename F>
    bool bind(const MsgPack::str_t& name, F&& func, MsgPack::str_t tag=""){
        return dispatcher.bind(name, func, tag);
    }

    bool hasTag(MsgPack::str_t name, MsgPack::str_t tag){
        return dispatcher.hasTag(name, tag);
    }

    void run() {

        RPCRequest req;
        if (get_rpc(req)) {         // Populate local request
            process_request(req);   // Process local data
            send_response(req);     // Send from local data
        }

    }

    bool get_rpc(RPCRequest& req, MsgPack::str_t tag="") {
        decoder->decode();

        MsgPack::str_t method = decoder->fetch_method();

        if (method == "" || !hasTag(method, tag)) return false;

        req.size = decoder->get_request(req.buffer, RPC_BUFFER_SIZE);
        return req.size > 0;
    }

    void process_request(RPCRequest& req) {
        if (req.size == 0) return;

        MsgPack::Unpacker unpacker;

        unpacker.clear();
        if (!unpacker.feed(req.buffer, req.size)) return;

        int msg_type;
        int msg_id;
        MsgPack::str_t method;
        MsgPack::arr_size_t req_size;

        if (!unpacker.deserialize(req_size, msg_type)) {
            reset_rpc();
            return; // Header not unpackable
        }

        if (msg_type == CALL_MSG && req_size.size() == REQUEST_SIZE) {
            if (!unpacker.deserialize(msg_id, method)) {
                req.reset();
                return; // Method not unpackable
            }
        } else if (msg_type == NOTIFY_MSG && req_size.size() == NOTIFY_SIZE) {
            if (!unpacker.deserialize(method)) {
                req.reset();
                return; // Method not unpackable
            }
        } else {
            req.reset();
            return; // Invalid request size/type
        }

        req.type = msg_type;

        MsgPack::arr_size_t resp_size(RESPONSE_SIZE);
        req.res_packer.clear();
        if (msg_type == CALL_MSG) req.res_packer.serialize(resp_size, RESP_MSG, msg_id);

        dispatcher.call(method, unpacker, req.res_packer);

    }

    bool send_response(RPCRequest& req) {

        if (req.type == NO_MSG || req.res_packer.size() == 0) {
            return true; // No response to send
        }

        if (req.type == NOTIFY_MSG) return true;

        return decoder->send_response(req.res_packer);

    }

private:
    RpcDecoder<>* decoder = nullptr;
    RpcFunctionDispatcher<MAX_CALLBACKS> dispatcher;
    uint8_t _rpc_buffer[RPC_BUFFER_SIZE];
    size_t _rpc_size = 0;
    int _rpc_type = NO_MSG;
    MsgPack::Packer res_packer;
    
    void reset_rpc() {
        _rpc_size = 0;
        _rpc_type = NO_MSG;
    }

};

#endif //RPCLITE_SERVER_H
