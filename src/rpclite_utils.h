#pragma once
#ifndef RPCLITE_UTILS_H
#define RPCLITE_UTILS_H

#include <tuple>
#include <utility> 

namespace RpcUtils {
namespace detail {

template<typename T>
bool deserialize_single(MsgPack::Unpacker& unpacker, T& value) {
    if (!unpacker.unpackable(value)) return false;
    unpacker.deserialize(value);
    return true;
}

template<std::size_t I = 0, typename... Ts>
typename std::enable_if<I == sizeof...(Ts), bool>::type
deserialize_tuple(MsgPack::Unpacker&, std::tuple<Ts...>&) {
    return true;
}

template<std::size_t I = 0, typename... Ts>
typename std::enable_if<I < sizeof...(Ts), bool>::type
deserialize_tuple(MsgPack::Unpacker& unpacker, std::tuple<Ts...>& out) {
    if (!deserialize_single(unpacker, std::get<I>(out))) return false;
    return deserialize_tuple<I + 1>(unpacker, out);
}

template<typename... Ts>
bool deserialize_all(MsgPack::Unpacker& unpacker, std::tuple<Ts...>& values) {
    return deserialize_tuple(unpacker, values);
}

// Helper to invoke a function with a tuple of arguments
template<typename F, typename Tuple, std::size_t... I>
auto invoke_with_tuple(F&& f, Tuple&& t, arx::stdx::index_sequence<I...>)
    -> decltype(f(std::get<I>(std::forward<Tuple>(t))...)) {
    return f(std::get<I>(std::forward<Tuple>(t))...);
}

} // namespace detail
} // namespace RpcUtils

#endif //RPCLITE_UTILS_H