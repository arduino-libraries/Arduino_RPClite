// Created by lucio on 4/25/25.
//

#ifndef ERROR_RPC_H
#define ERROR_RPC_H

#include "MsgPack.h"

struct RpcError {
    int code;
    MsgPack::str_t traceback;
    MSGPACK_DEFINE(code, traceback); // -> [code, traceback]
};

#endif