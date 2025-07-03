#ifndef RPCLITE_CLIENT_H
#define RPCLITE_CLIENT_H
#include "error.h"
#include "decoder_manager.h"
#include "SerialTransport.h"


class RPCClient {
    RpcDecoder<>* decoder = nullptr;
    int _waiting_msg_id;

public:
    RpcError lastError;

    RPCClient(ITransport& t) : decoder(&RpcDecoderManager<>::getDecoder(t)) {}

    RPCClient(Stream& stream) {
        ITransport* transport = (ITransport*) new SerialTransport(stream);
        decoder = &RpcDecoderManager<>::getDecoder(*transport);
    }

    template<typename... Args>
    void notify(const MsgPack::str_t method, Args&&... args)  {
        int _id;
        decoder->send_call(NOTIFY_MSG, method, _id, std::forward<Args>(args)...);
    }

    template<typename RType, typename... Args>
    bool call(const MsgPack::str_t method, RType& result, Args&&... args) {

        if(!send_rpc(method, std::forward<Args>(args)...)) {
            lastError.code = GENERIC_ERR;
            lastError.traceback = "Failed to send RPC call";
            return false;
        }

        // blocking call
        while (!get_response(result)){
            //delay(1);
        }

        return (lastError.code == NO_ERR);

    }

protected:
    template<typename... Args>
    bool send_rpc(const MsgPack::str_t method, Args&&... args) {
        int msg_id;
        if (decoder->send_call(CALL_MSG, method, msg_id, std::forward<Args>(args)...)) {
            _waiting_msg_id = msg_id;
            return true;
        }
        return false;
    }

    template<typename RType>
    bool get_response(RType& result) {
        RpcError tmp_error;
        decoder->decode();

        if (decoder->get_response(_waiting_msg_id, result, tmp_error)) {
            lastError.code = tmp_error.code;
            lastError.traceback = tmp_error.traceback;
            return true;
        }
        return false;
    }

};

#endif //RPCLITE_CLIENT_H
