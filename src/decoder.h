#ifndef RPCLITE_DECODER_H
#define RPCLITE_DECODER_H

#include "MsgPack.h"
#include "transport.h"
#include "dispatcher.h"


#define NO_MSG          -1
#define CALL_MSG        0
#define RESP_MSG        1
#define NOTIFY_MSG      2

#define REQUEST_SIZE    4
#define RESPONSE_SIZE   4
#define NOTIFY_SIZE     3

#define MAX_BUFFER_SIZE 1024
#define CHUNK_SIZE      32

template<size_t BufferSize = MAX_BUFFER_SIZE>
class RpcDecoder {

public:
    RpcDecoder(ITransport& transport) : _transport(transport) {}

    template<typename... Args>
    bool send_call(const int call_type, const MsgPack::str_t method, int& msg_id, Args&&... args) {

        if (call_type!=CALL_MSG && call_type!=NOTIFY_MSG) return false;

        static MsgPack::Packer packer;
        packer.clear();

        if (call_type == CALL_MSG){
            msg_id = _msg_id;
            MsgPack::arr_size_t call_size(REQUEST_SIZE);
            packer.serialize(call_size, call_type, msg_id, method);
        } else {
            MsgPack::arr_size_t call_size(NOTIFY_SIZE);
            packer.serialize(call_size, call_type, method);
        }

        MsgPack::arr_size_t arg_size(sizeof...(args));
        packer.serialize(arg_size, std::forward<Args>(args)...);

#ifdef DEBUG
        Serial.print("Sending: ");
        for (size_t i=0; i<packer.size(); i++){
            Serial.print(packer.data()[i], HEX);
            Serial.print(" ");
        }
        Serial.println(" ");
#endif

        if (send(reinterpret_cast<const uint8_t*>(packer.data()), packer.size()) == packer.size()){
            _msg_id++;
            return true;
        }
        return false;
    }

    template<typename RType>
    bool get_response(const int msg_id, RType& result, RpcError& error) {

        if (!packet_incoming() || packet_type()!=RESP_MSG) return false;

        static MsgPack::Unpacker unpacker;

        size_t bytes_checked = 0;

        while (bytes_checked < _bytes_stored) {
            bytes_checked++;
            unpacker.clear();
            if (!unpacker.feed(_raw_buffer, bytes_checked)) continue;
            MsgPack::arr_size_t resp_size;
            int resp_type;
            int resp_id;
            if (!unpacker.deserialize(resp_size, resp_type, resp_id)) continue;
            if (resp_size.size() != RESPONSE_SIZE) continue;
            if (resp_type != RESP_MSG) continue;
            if (resp_id != msg_id) continue;

            MsgPack::object::nil_t nil;
            if (unpacker.unpackable(nil)){  // No error
                if (!unpacker.deserialize(nil, result)) continue;
            } else {                        // RPC returned an error
                if (!unpacker.deserialize(error, nil)) continue;
            }
            pop_packet(bytes_checked);
            return true;
        }
#ifdef DEBUG
        print_buffer();
#endif
        return false;
    }

    template<typename RType>
    bool send_response(const int msg_id, const RpcError& error, const RType& result) {
        static MsgPack::Packer packer;
        MsgPack::arr_size_t resp_size(RESPONSE_SIZE);
        MsgPack::object::nil_t nil;

        packer.clear();
        packer.serialize(resp_size, RESP_MSG, msg_id);

        if (error.code == NO_ERR){
            packer.serialize(nil, result);
        } else {
            packer.serialize(error, nil);
        }

        return send(reinterpret_cast<const uint8_t*>(packer.data()), packer.size()) == packer.size();

    }

    template<size_t N>
    void process_requests(RpcFunctionDispatcher<N>& dispatcher) {
        if (_packet_type!=CALL_MSG && _packet_type!=NOTIFY_MSG) return;

        static MsgPack::Unpacker unpacker;
        static MsgPack::Packer packer;

        size_t bytes_checked = 0;

        while (bytes_checked < _bytes_stored) {
            bytes_checked++;
            unpacker.clear();
            if (!unpacker.feed(_raw_buffer, bytes_checked)) continue;

            int msg_type;
            int msg_id;
            MsgPack::str_t method;
            MsgPack::arr_size_t req_size;

            if (!unpacker.deserialize(req_size, msg_type)) break;
            // todo HANDLE MALFORMED CLIENT REQ ERRORS
            if ((req_size.size() == REQUEST_SIZE) && (msg_type == CALL_MSG)){
                if (!unpacker.deserialize(msg_id, method)) {
                    discard_packet();
                    break;
                }
            } else if ((req_size.size() == NOTIFY_SIZE) && (msg_type == NOTIFY_MSG)) {
                if (!unpacker.deserialize(method)) {
                    discard_packet();
                    break;
                }
            } else if ((req_size.size() == RESPONSE_SIZE) && (msg_type == RESP_MSG)) {   // this should never happen
                break;
            } else {
                discard_packet();
                break;
            }

            MsgPack::arr_size_t resp_size(RESPONSE_SIZE);
            packer.clear();
            packer.serialize(resp_size, RESP_MSG, msg_id);
            size_t headers_size = packer.size();

            if (!dispatcher.call(method, unpacker, packer)) {
                if (packer.size()==headers_size) {
                    // Call didn't go through bc parameters are not ready yet
                    continue;
                } else {
                    // something went wrong the call raised an error or the client issued a malformed request
                    if (msg_type == CALL_MSG) {
                        send(reinterpret_cast<const uint8_t*>(packer.data()), packer.size()) == packer.size();
                    }   // if notification client will never know something went wrong
                    discard_packet();   // agnostic pop
                    break;
                }
            } else {
                // all is well we can respond and pop the deserialized packet
                if (msg_type == CALL_MSG){
                    send(reinterpret_cast<const uint8_t*>(packer.data()), packer.size()) == packer.size();
                }
                pop_packet(bytes_checked);
                break;
            }

        }

    }

    void process(){
        if (advance()) parse_packet();
    }

    // Fill the raw buffer to its capacity
    bool advance() {

        uint8_t temp_buf[CHUNK_SIZE];
    
        if (_transport.available() && !buffer_full()){
            int bytes_read = _transport.read(temp_buf, CHUNK_SIZE);
            if (bytes_read <= 0) return false;
    
            for (int i = 0; i < bytes_read; ++i) {
                _raw_buffer[_bytes_stored] = temp_buf[i];
                _bytes_stored++;
                while (buffer_full()){
                    delay(1);
                }
            }
        }
        return true;
    }

    void parse_packet(){

        if (packet_incoming() || buffer_empty()){return;}

        static MsgPack::Unpacker unpacker;
        unpacker.clear();
        unpacker.feed(_raw_buffer, 2);

        MsgPack::arr_size_t elem_size;
        int type;
        if (unpacker.deserialize(elem_size, type)){
            _packet_type = type;
        }

    }

    // Check if a packet is available
    inline bool packet_incoming() const { return _packet_type >= CALL_MSG; }

    int packet_type() const {return _packet_type;}

    // Get the oldest packet (returns false if no packet available)
    bool get_next_packet(MsgPack::Unpacker& unpacker, size_t size) {
        if (!packet_incoming()) return false;
        unpacker.clear();
        return unpacker.feed(_raw_buffer, size);
    }

    // Try to recover buffer error condition
    void recover() {
        // ensure parsing was attempted
        parse_packet();
        if (buffer_full() && !packet_incoming()){
            flush_buffer();
        }
    }

    // Discard the oldest packet. Returns the number of freed_bytes
    size_t pop_packet(size_t size) {

        if (size > _bytes_stored) return 0;

        const size_t remaining_bytes = _bytes_stored - size;

        // Shift remaining data forward (manual memmove for compatibility)
        for (size_t i = 0; i < remaining_bytes; i++) {
            _raw_buffer[i] = _raw_buffer[size + i];
        }

        _bytes_stored = remaining_bytes;
        _packet_type = NO_MSG;

        return size;
    }

    size_t discard_packet() {
        // todo recursive packet size measurement
        // use pop_packet with size
        // return popped bytes
        return 0;
    }

    inline size_t size() const {return _bytes_stored;}

#ifdef DEBUG
void print_buffer(){

    Serial.print("buf size: ");
    Serial.print(_bytes_stored);
    Serial.print(" : ");

    for (size_t i = 0; i < _bytes_stored; i++) {
        Serial.print(_raw_buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}
#endif

private:
    ITransport& _transport;
    uint8_t _raw_buffer[BufferSize];
    size_t _bytes_stored = 0;
    int _packet_type = NO_MSG;
    int _msg_id = 0;

    inline bool buffer_full() const { return _bytes_stored == BufferSize; }
    inline bool buffer_empty() const { return _bytes_stored == 0;}
    inline void flush_buffer() {
        uint8_t* discard_buf;
        while (_transport.read(discard_buf, CHUNK_SIZE) > 0);
        _bytes_stored = 0;
    }
    inline size_t send(const uint8_t* data, const size_t size) {
        return _transport.write(data, size);
    }
};

#endif