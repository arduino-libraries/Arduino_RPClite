//
// Created by lucio on 4/8/25.
//

#ifndef SERIALTRANSPORT_H
#define SERIALTRANSPORT_H
#include "transport.h"

class SerialTransport: public ITransport {

    HardwareSerial* uart;

    public:

        SerialTransport(HardwareSerial* _uart): uart(_uart){}

        void begin(){
            uart->begin(115200);
        }

        size_t write(const uint8_t* data, size_t size) override {

            for (size_t i=0; i<size; i++){
                uart->write(data[i]);
            }

            return size;
        }

        size_t read(uint8_t* buffer, size_t size) override {
            
            size_t r_size = uart->available();

            if (r_size == 0){return r_size;}    // UNAVAILABLE

            if (r_size > size){
                return r_size;  // ERROR
            }

            for (size_t i=0; i<r_size; i++){
                buffer[i] = uart->read();
            }

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