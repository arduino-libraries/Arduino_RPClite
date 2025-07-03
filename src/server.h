#ifndef RPCLITE_SERVER_H
#define RPCLITE_SERVER_H

#include "error.h"
#include "wrapper.h"
#include "dispatcher.h"
#include "decoder.h"
#include "decoder_manager.h"
#include "SerialTransport.h"

#define MAX_CALLBACKS   100
#define RPC_BUFFER_SIZE  1024

class RPCServer {

public:
    RPCServer(ITransport& t) : decoder(&RpcDecoderManager<>::getDecoder(t)) {}

    RPCServer(Stream& stream) {
        ITransport* transport = (ITransport*) new SerialTransport(stream);
        decoder = &RpcDecoderManager<>::getDecoder(*transport);
    }

    template<typename F>
    bool bind(const MsgPack::str_t& name, F&& func){
        return dispatcher.bind(name, func);
    }

    void run() {
        get_rpc();
        process_request();
        send_response();
        //delay(1);
    }

protected:
    void get_rpc() {
        decoder->decode();
        if (_rpc_size > 0) return; // Already have a request
        _rpc_size = decoder->get_request(_rpc_buffer, RPC_BUFFER_SIZE);
    }

    void process_request() {
        if (_rpc_size == 0) return;

        MsgPack::Unpacker unpacker;

        unpacker.clear();
        if (!unpacker.feed(_rpc_buffer, _rpc_size)) {
            _rpc_size = 0; // Reset size on error
            return; // Error in unpacking
        }

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
                reset_rpc();
                return; // Method not unpackable
            }
        } else if (msg_type == NOTIFY_MSG && req_size.size() == NOTIFY_SIZE) {
            if (!unpacker.deserialize(method)) {
                reset_rpc();
                return; // Method not unpackable
            }
        } else {
            reset_rpc();
            return; // Invalid request size/type
        }

        _rpc_type = msg_type;

        MsgPack::arr_size_t resp_size(RESPONSE_SIZE);
        res_packer.clear();
        if (msg_type == CALL_MSG) res_packer.serialize(resp_size, RESP_MSG, msg_id);

        dispatcher.call(method, unpacker, res_packer);

    }

    bool send_response() {
        if (_rpc_type == NO_MSG || res_packer.size() == 0) {
            return true; // No response to send
        }

        if (_rpc_type == NOTIFY_MSG) {
            reset_rpc();
            return true;
        }

        reset_rpc();
        return decoder->send_response(res_packer);

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
