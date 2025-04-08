//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_CLIENT_H
#define RPCLITE_CLIENT_H
#include "rpc.h"

class RPCClient {
    ITransport& transport;
    int request_id = 0;

public:
    RPCClient(ITransport& t) : transport(t) {}

    float call(const std::string& method, float a, float b) {

        //msgpack::sbuffer buffer;
        //msgpack::packer<msgpack::sbuffer> pk(&buffer);
        //pk.pack_map(3);
        //pk.pack("method"); pk.pack(method);
        //pk.pack("args");   pk.pack_array(2); pk.pack(a); pk.pack(b);
        //pk.pack("id");     pk.pack(++request_id);

        MsgPack::Packer packer;
        MsgPack::map_t<String, float> request;

        request["first"] = a;
        request["second"] = b;

        packer.serialize(request);

        send_msg(transport, packer.buffer);

        MsgPack::Unpacker unpacker;
        if (recv_msg(transport, unpacker)) {
            MsgPack::map_t<String, float> resp;
            unpacker.deserialize(resp);
            return resp["result"];
        }

        //msgpack::unpacked response;
        //if (recv_msg(transport, response)) {
        //    auto res = response.get().as<std::map<std::string, msgpack::object>>();
        //    return res["result"].as<int>();
        //}

        return -1; // Error TODO this is clearly wrong. Client should be waiting for reponse + timeout etc.
    }
};

#endif //RPCLITE_CLIENT_H
