#ifndef RPCLITE_DECODER_H
#define RPCLITE_DECODER_H
#include "transport.h"
#include "MsgPack.h"


#define CALL_MSG        0
#define RESP_MSG        1
#define NOTIFY_MSG      2

#define MAX_BUFFER_SIZE 1024
#define CHUNK_SIZE      32

template<size_t BufferSize = MAX_BUFFER_SIZE>
class RpcDecoder {

public:
    RpcDecoder(ITransport& transport) : _transport(transport) {}


    void process(){
        if (advance()) return parse_packet();
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

    // Tries to parse the first packet only
    void parse_packet(){

        // Nop in case 1st packet is ready... waiting to deliver
        if (_packet_ready){return;}

        size_t bytes_checked = 0;

        MsgPack::Unpacker unpacker;
        while (bytes_checked < _bytes_stored) {
            if (unpacker.feed(_raw_buffer, bytes_checked)) {
                _packet_ready = true;
                _packet_size = bytes_checked + 1;
                return;
            } else {
                bytes_checked++;
            }
        }

    }

    // Check if a packet is available
    bool packet_available() const { return _packet_ready; }

    // Get the oldest packet (returns false if no packet available)
    bool get_next_packet(MsgPack::Unpacker& unpacker) {
        if (!_packet_ready) return false;
        return unpacker.feed(_raw_buffer, _packet_size);
    }

    // Try to recover buffer error condition
    void recover() {
        // ensure parsing was attempted
        parse_packet();
        if (buffer_full() && !_packet_ready){
            flush_buffer();
        }
    }

    // Discard the oldest packet. Returns the number of freed_bytes
    size_t pop_packet() {

        if (!_packet_ready) return false;

        const size_t remaining_bytes = _bytes_stored - _packet_size;

        // Shift remaining data forward (manual memmove for compatibility)
        for (size_t i = 0; i < remaining_bytes; i++) {
            _raw_buffer[i] = _raw_buffer[_packet_size + i];
        }

        _bytes_stored = remaining_bytes;
        _packet_ready = false;
        _packet_size = 0; // Reset packet state
        
        return _packet_size;
    }

private:
    ITransport& _transport;
    uint8_t _raw_buffer[BufferSize];
    size_t _bytes_stored = 0;

    bool _packet_ready = false;
    size_t _packet_size = 0;

    inline bool buffer_full() const { return _bytes_stored == BufferSize; }
    inline bool buffer_empy() const { return _bytes_stored == 0;}
    inline void flush_buffer() {
        uint8_t* discard_buf;
        while (_transport.read(discard_buf, CHUNK_SIZE) > 0);
        _bytes_stored = 0;
    }
};

#endif