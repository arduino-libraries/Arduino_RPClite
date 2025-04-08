//
// Created by lucio on 4/8/25.
//

#ifndef RPCLITE_SERVER_H
#define RPCLITE_SERVER_H

#include "rpc.h"
//#include <unordered_map>
//#include <functional>
//#include <string>

class RPCServer {
    ITransport& transport;
    //std::unordered_map<std::string, std::function<int(int, int)>> handlers;

public:
    RPCServer(ITransport& t) : transport(t) {}

    //void bind(const std::string& name, std::function<int(int, int)> fn) {
        //handlers[name] = fn;
    //}

    void loop() {
        MsgPack::Unpacker unpacker;
        if (!recv_msg(transport, unpacker)) return;

        MsgPack::map_t<String, float> rm;
        unpacker.deserialize(rm);

        //auto obj = msg.get();
        //auto map = obj.as<std::map<std::string, msgpack::object>>();

        //std::string method = map["method"].as<std::string>();
        //auto args = map["args"].as<std::vector<int>>();
        //int id = map["id"].as<int>();

        //int result = handlers[method](args[0], args[1]);

        float result = rm["first"] + rm["second"];

        //msgpack::sbuffer buffer;
        //msgpack::packer<msgpack::sbuffer> pk(&buffer);
        //pk.pack_map(2);
        //pk.pack("result"); pk.pack(result);
        //pk.pack("id");     pk.pack(id);

        MsgPack::Packer packer;
        MsgPack::map_t<String, float> resp;

        resp["result"] = result;
        packer.serialize(resp);

        send_msg(transport, packer.buffer);
    }
};

#endif //RPCLITE_SERVER_H
