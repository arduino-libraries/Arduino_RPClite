/*
    This file is part of the Arduino_RPClite library.

    Copyright (c) 2025 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    
*/

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

        RPCRequest<> req;

        if (!get_rpc(req)) return; // Populate local request

        process_request(req);   // Process local data

        send_response(req);     // Send from local data

    }

    bool get_rpc(RPCRequest<>& req, MsgPack::str_t tag="") {
        decoder->decode();

        MsgPack::str_t method = decoder->fetch_rpc_method();

        if (method == "" || !hasTag(method, tag)) return false;

        req.size = decoder->get_request(req.buffer, req.get_buffer_size());   // todo overload get_request(RPCRequest& req) so all the request info is in req
        return req.size > 0;
    }

    void process_request(RPCRequest<>& req) {

        if (!req.unpack_request_headers()) {
            req.reset();
            return;
        }

        req.pack_response_headers();

        dispatcher.call(req.method, req.unpacker, req.packer);

    }

    bool send_response(RPCRequest<>& req) {

        if (req.type == NO_MSG || req.packer.size() == 0) {
            return true; // No response to send
        }

        if (req.type == NOTIFY_MSG) return true;

        return decoder->send_response(req.packer);

    }

private:
    RpcDecoder<>* decoder = nullptr;
    RpcFunctionDispatcher<MAX_CALLBACKS> dispatcher;
    
};

#endif //RPCLITE_SERVER_H
