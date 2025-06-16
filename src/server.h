//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_SERVER_H
#define RPCLITE_SERVER_H

#include "error.h"
#include "wrapper.h"
#include "dispatcher.h"
#include "decoder.h"
#include "decoder_manager.h"
#include "SerialTransport.h"

#define MAX_CALLBACKS   100

class RPCServer {
    RpcDecoder<>* decoder = nullptr;
    RpcFunctionDispatcher<MAX_CALLBACKS> dispatcher;

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
        decoder->decode();
        decoder->process_requests(dispatcher);
        delay(1);
    }

};

#endif //RPCLITE_SERVER_H
