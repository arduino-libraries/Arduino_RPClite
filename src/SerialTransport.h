//
// Created by lucio on 4/8/25.
//

#ifndef SERIALTRANSPORT_H
#define SERIALTRANSPORT_H
#include "transport.h"

class SerialTransport: public ITransport {

    Stream* _stream;

    public:

        SerialTransport(Stream* stream): _stream(stream){}

        SerialTransport(Stream& stream): _stream(&stream){}

        void begin(){}

        bool available() override {
            return _stream->available();
        }

        size_t write(const uint8_t* data, size_t size) override {

            for (size_t i=0; i<size; i++){
                _stream->write(data[i]);
            }

            return size;
        }

        size_t read(uint8_t* buffer, size_t size) override {
            _stream->setTimeout(0);
            return _stream->readBytes(buffer, size);
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