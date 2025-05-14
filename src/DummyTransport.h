//
// Created by lucio on 4/8/25.
//

#ifndef DUMMY_TRANSPORT_H
#define DUMMY_TRANSPORT_H
#include "transport.h"

class DummyTransport: public ITransport {

    const uint8_t* _dummy_buf;
    size_t _dummy_buf_size = 0;

    public:

        DummyTransport(const uint8_t* buf, size_t size): _dummy_buf(buf), _dummy_buf_size(size){}

        void begin(){}

        bool available() override {
            return _dummy_buf_size > 0;
        }

        size_t write(const uint8_t* data, size_t size) override {
            // Mock write
            return size;
        }

        size_t read(uint8_t* buffer, size_t size) override {
            if (_dummy_buf_size == 0) return 0;

            size_t r_size = 0;
            for (size_t i = 0; i < _dummy_buf_size; i++){
                buffer[r_size] = _dummy_buf[i];
                delay(1);
                r_size++;
                if (r_size == size) break;
            }

            _dummy_buf_size = _dummy_buf_size - r_size;
            return r_size;

        }

        size_t read_byte(uint8_t& r) override {
            uint8_t b[1];
            if (read(b, 1) != 1){
                return 0;
            };
            r = b[0];
            return 1;
        }

};

#endif  //SERIALTRANSPORT_H