/*
    This file is part of the Arduino_RPClite library.

    Copyright (c) 2025 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    
*/

#ifndef RPCLITE_WRAPPER_H
#define RPCLITE_WRAPPER_H

#include "error.h"
#include "rpclite_utils.h"

using namespace RpcUtils::detail;

#ifdef HANDLE_RPC_ERRORS
#include <stdexcept>
#endif
#ifdef ARDUINO_ARCH_ZEPHYR
#include <functional>
#endif

class IFunctionWrapper {
public:
    virtual ~IFunctionWrapper() {}
    virtual bool operator()(MsgPack::Unpacker& unpacker, MsgPack::Packer& packer) = 0;
};

#ifdef ARDUINO_ARCH_ZEPHYR
template<typename R, typename... Args>
class RpcFunctionWrapper: public IFunctionWrapper {
#else
template<typename F>
class RpcFunctionWrapper;

template<typename R, typename... Args>
class RpcFunctionWrapper<std::function<R(Args...)>>: public IFunctionWrapper {
#endif
public:
    RpcFunctionWrapper(std::function<R(Args...)> func) : _func(func) {}

    R operator()(Args... args) {
        return _func(args...);
    }

    bool operator()(MsgPack::Unpacker& unpacker, MsgPack::Packer& packer) override {

        MsgPack::object::nil_t nil;

#ifdef HANDLE_RPC_ERRORS
    try {
#endif

        // First check the parameters size
        if (!unpacker.isArray()){
            RpcError error(MALFORMED_CALL_ERR, "Unserializable parameters array");
            error.to_msgpack(packer);
            return false;
        }

        MsgPack::arr_size_t param_size;

        unpacker.deserialize(param_size);
        if (param_size.size() < sizeof...(Args)){
            RpcError error(MALFORMED_CALL_ERR, "Missing call parameters (WARNING: Default param resolution is not implemented)");
            error.to_msgpack(packer);
            return false;
        }

        if (param_size.size() > sizeof...(Args)){
            RpcError error(MALFORMED_CALL_ERR, "Too many parameters");
            error.to_msgpack(packer);
            return false;
        }

        return handle_call(unpacker, packer);

#ifdef HANDLE_RPC_ERRORS
    } catch (const std::exception& e) {
        // Should be specialized according to the exception type
        RpcError error(GENERIC_ERR, "RPC error");
        packer.serialize(error, nil);
        return false;
    }
#endif

    }

private:
    std::function<R(Args...)> _func;

    template<typename Dummy = R>
    typename std::enable_if<std::is_void<Dummy>::value, bool>::type
    handle_call(MsgPack::Unpacker& unpacker, MsgPack::Packer& packer) {
        //unpacker not ready if deserialization fails at this point
        std::tuple<Args...> args;
        if (!deserialize_tuple(unpacker, args)) return false;
        MsgPack::object::nil_t nil;
        invoke_with_tuple(_func, args, arx::stdx::make_index_sequence<sizeof...(Args)>{});
        packer.serialize(nil, nil);
        return true;
    }

    template<typename Dummy = R>
    typename std::enable_if<!std::is_void<Dummy>::value, bool>::type
    handle_call(MsgPack::Unpacker& unpacker, MsgPack::Packer& packer) {
        //unpacker not ready if deserialization fails at this point
        std::tuple<Args...> args;
        if (!deserialize_tuple(unpacker, args)) return false;
        MsgPack::object::nil_t nil;
        R out = invoke_with_tuple(_func, args, arx::stdx::make_index_sequence<sizeof...(Args)>{});
        packer.serialize(nil, out);
        return true;
    }
};

#ifdef ARDUINO_ARCH_ZEPHYR
template<typename F>
auto wrap(F&& f) {
    return new RpcFunctionWrapper(std::function(std::forward<F>(f)));
};
#else
template<typename F, typename Signature = typename arx::function_traits<typename std::decay<F>::type>::function_type>
auto wrap(F&& f) -> RpcFunctionWrapper<Signature>* {
    return new RpcFunctionWrapper<Signature>(std::forward<F>(f));
};
#endif

#endif