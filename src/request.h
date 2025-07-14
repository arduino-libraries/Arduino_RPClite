#ifndef RPCLITE_REQUEST_H
#define RPCLITE_REQUEST_H

#define RPC_BUFFER_SIZE  1024


#include "rpclite_utils.h"

class RPCRequest {

public:
    uint8_t buffer[RPC_BUFFER_SIZE];
    size_t size = 0;
    int type = NO_MSG;
    MsgPack::Packer res_packer;

    void reset(){
        size = 0;
        type = NO_MSG;
        res_packer.clear();
    }

};

#endif RPCLITE_REQUEST_H