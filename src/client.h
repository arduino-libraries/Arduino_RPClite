//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_CLIENT_H
#define RPCLITE_CLIENT_H
#include "error.h"
#include "decoder_manager.h"

class RPCClient {
    ITransport& transport;
    RpcDecoder<>& decoder;

public:
    RPCClient(ITransport& t) : transport(t), decoder(RpcDecoderManager<>::getDecoder(t)) {}

    template<typename... Args>
    void notify(const MsgPack::str_t method, Args&&... args)  {
        int _id;
        decoder.send_call(NOTIFY_MSG, method, _id, std::forward<Args>(args)...);
    }

    template<typename RType, typename... Args>
    bool call(const MsgPack::str_t method, RType& result, Args&&... args) {

        int msg_id;
        if (!decoder.send_call(CALL_MSG, method, msg_id, std::forward<Args>(args)...)){
        }

        RpcError error;
        // blocking call
        while (!decoder.get_response(msg_id, result, error)){
            decoder.process();
            delay(1);
        }

#ifdef DEBUG
        if (error.code != NO_ERR){
            //Serial.print("Server-side error message: ");
            //Serial.println(error.traceback);
        }
#endif

        return (error.code == NO_ERR);

    }
};

#endif //RPCLITE_CLIENT_H
