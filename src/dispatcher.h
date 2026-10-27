/*
    This file is part of the Arduino_RPClite library.

    Copyright (c) 2025 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    
*/

#ifndef RPCLITE_DISPATCHER_H
#define RPCLITE_DISPATCHER_H

#include "wrapper.h"
#include "error.h"

struct DispatchEntry {
    MsgPack::str_t name;
    MsgPack::str_t tag;
    IFunctionWrapper* fn;
};

template<size_t N>
class RpcFunctionDispatcher {
public:

    RpcFunctionDispatcher() = default;

    RpcFunctionDispatcher(const RpcFunctionDispatcher&) = delete;
    RpcFunctionDispatcher& operator=(const RpcFunctionDispatcher&) = delete;

    template<typename F>
    bool bind(MsgPack::str_t name, F&& f, MsgPack::str_t tag="") {
        if (_entryCount >= N) return false;

        if (findEntryIndex(name) < _entryCount) return false;

        _entries[_entryCount] = {name, tag, wrap(std::forward<F>(f))};
        _whitelist[_count++] = _entryCount++;

        return true;
    }

    bool unbind(const MsgPack::str_t& name) {
        const size_t entryIndex = findEntryIndex(name);

        if (entryIndex >= _entryCount) return false;

        for (size_t i = 0; i < _count; ++i) {
            if (_whitelist[i] == entryIndex) {
                _whitelist[i] = _whitelist[--_count];
                return true;
            }
        }

        return false;
    }

    bool rebind(const MsgPack::str_t& name) {
        const size_t entryIndex = findEntryIndex(name);
        if (entryIndex >= _entryCount) return false;
        return whitelist(entryIndex);
    }

    bool isBound(const MsgPack::str_t& name) const {
        const size_t entryIndex = findEntryIndex(name);

        if (entryIndex >= _entryCount) return false;

        return isWhitelisted(entryIndex);
    }

    bool hasTag(const MsgPack::str_t& name, MsgPack::str_t& tag) const {
        for (size_t i = 0; i < _count; ++i) {
            const size_t entryIndex = _whitelist[i];

            if (_entries[entryIndex].name == name && _entries[entryIndex].tag == tag) {
                return true;
            }
        }

        return false;
    }

    bool call(const MsgPack::str_t& name, MsgPack::Unpacker& unpacker, MsgPack::Packer& packer) {
        for (size_t i = 0; i < _count; ++i) {
            const size_t entryIndex = _whitelist[i];

            if (_entries[entryIndex].name == name) {
                return (*_entries[entryIndex].fn)(unpacker, packer);
            }
        }

        // handler not found
        MsgPack::object::nil_t nil;
        packer.serialize(RpcError(FUNCTION_NOT_FOUND_ERR, name), nil);
        return false;
    }

private:
    size_t findEntryIndex(const MsgPack::str_t& name) const {
        for (size_t i = 0; i < _entryCount; ++i) {
            if (_entries[i].name == name) {
                return i;
            }
        }

        return N;
    }

    bool isWhitelisted(size_t entryIndex) const {
        for (size_t i = 0; i < _count; ++i) {
            if (_whitelist[i] == entryIndex) {
                return true;
            }
        }

        return false;
    }

    bool whitelist(size_t entryIndex) {
        if (isWhitelisted(entryIndex)) {
            return true;
        }

        if (_count >= N) {
            return false;
        }

        _whitelist[_count++] = entryIndex;
        return true;
    }

    DispatchEntry _entries[N];
    size_t _whitelist[N];
    size_t _entryCount = 0;
    size_t _count = 0;
};

#endif