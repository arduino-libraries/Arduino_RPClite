#ifndef RPCLITE_DISPATCHER_H
#define RPCLITE_DISPATCHER_H

#include <map>
#include "wrapper.h"
#include "error.h"

class RpcFunctionDispatcher {
    public:
        template<typename F>
        void bind(const MsgPack::str_t& name, F&& func) {
            _functions[name] = std::make_shared<decltype(wrap(std::forward<F>(func)))>(wrap(std::forward<F>(func)));
        }
    
        bool call(const MsgPack::str_t& name, MsgPack::Unpacker& unpacker, MsgPack::Packer& packer) {
            auto it = _functions.find(name);
            if (it != _functions.end()) {
                return (*(it->second))(unpacker, packer);
            }

            RpcError not_found(FUNCTION_NOT_FOUND_ERR, name);
            MsgPack::object::nil_t nil;
            packer.serialize(not_found, nil);

            return false;
        }
    
    private:
        std::map<MsgPack::str_t, std::shared_ptr<IFunctionWrapper>> _functions;
    };

#endif