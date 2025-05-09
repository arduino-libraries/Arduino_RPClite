// Created by lucio on 4/25/25.
//

#ifndef RPCLITE_ERROR_H
#define RPCLITE_ERROR_H

#include "MsgPack.h"

#include "MsgPack.h"


#define FUNCTION_NOT_FOUND_ERR   0xFE
#define GENERIC_ERR              0xFF


struct RpcError {
    int code;
    MsgPack::str_t traceback;

    RpcError(int c, const MsgPack::str_t& tb)
        : code(c), traceback(tb) {}

    MSGPACK_DEFINE(code, traceback); // -> [code, traceback]
};

#endif