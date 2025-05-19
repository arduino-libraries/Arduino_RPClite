#ifndef RPCLITE_DECODER_H
#define RPCLITE_DECODER_H
#include "transport.h"
#include "MsgPack.h"


#define NO_MSG          -1
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

    inline bool buffer_full() const { return _bytes_stored == BufferSize; }
    inline bool buffer_empty() const { return _bytes_stored == 0;}
    inline void flush_buffer() {
        uint8_t* discard_buf;
        while (_transport.read(discard_buf, CHUNK_SIZE) > 0);
        _bytes_stored = 0;
    }
};

#endif