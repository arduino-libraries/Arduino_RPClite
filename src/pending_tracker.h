/*
    This file is part of the Arduino_RPClite library.

    Copyright (C) Arduino s.r.l. and/or its affiliated companies

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef RPCLITE_PENDING_TRACKER_H
#define RPCLITE_PENDING_TRACKER_H

#include <stddef.h>
#include <stdint.h>

#ifndef RPCLITE_MAX_PENDING_CALLS
#define RPCLITE_MAX_PENDING_CALLS  100
#endif

/**
 * Tracks the IDs of calls that have been sent and are waiting for a response.
 *
 * The decoder touches this container from both the TX path (send_call) and the
 * RX path (parse_packet / remove_pending_id). The default implementation is
 * NOT thread safe. Platforms with separate TX/RX locks (e.g. Zephyr) should
 * provide an implementation guarded by its own dedicated mutex.
 *
 * Implementations must keep critical sections short and must never call back
 * into the decoder, so the tracker lock stays a leaf lock.
 */
class IPendingTracker {
public:
    virtual ~IPendingTracker() = default;

    // Register an ID. Returns false if the tracker is full.
    virtual bool add(uint32_t id) = 0;

    // True if the ID is currently pending.
    // Non-const on purpose: locking implementations need to mutate a mutex.
    virtual bool contains(uint32_t id) = 0;

    // Remove the ID if present. Returns true if it was present.
    // This is an atomic "take": prefer it over contains() + remove().
    virtual bool remove(uint32_t id) = 0;

    virtual void clear() = 0;
};

template<size_t N = RPCLITE_MAX_PENDING_CALLS>
class PendingTracker : public IPendingTracker {
public:
    bool add(uint32_t id) override {
        if (_size >= N) return false;
        _ids[_size++] = id;
        return true;
    }

    bool contains(uint32_t id) override {
        for (size_t i = 0; i < _size; i++) {
            if (_ids[i] == id) return true;
        }
        return false;
    }

    bool remove(uint32_t id) override {
        for (size_t i = 0; i < _size; i++) {
            if (_ids[i] == id) {
                _ids[i] = _ids[--_size];
                return true;
            }
        }
        return false;
    }

    void clear() override { _size = 0; }

private:
    uint32_t _ids[N]{};
    size_t _size = 0;
};

#endif