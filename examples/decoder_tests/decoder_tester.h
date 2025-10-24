/*
    This file is part of the Arduino_RPClite library.

    Copyright (c) 2025 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    
*/

#pragma once
#ifndef RPCLITE_DECODER_TESTER_H
#define RPCLITE_DECODER_TESTER_H

class DecoderTester {
  
  RpcDecoder<>& decoder;

public:

  DecoderTester(RpcDecoder<>& _d): decoder(_d){}

  void first_response_info() {
    if (!decoder.response_queued()) {
      Serial.println("No response queued");
      return;
    }
    Serial.println("-- First response info --");
    Serial.print("RESP OFFSET: ");
    Serial.println(static_cast<int>(decoder._response_offset));
    Serial.print("RESP SIZE: ");
    Serial.println(static_cast<int>(decoder._response_size));
  }

  size_t get_response_size() {
    return decoder._response_size;
  }

  size_t get_response_offset() {
    return decoder._response_offset;
  }

  template<typename RType>
  bool get_response(const uint32_t msg_id, RType& result, RpcError& error) {
    return decoder.get_response(msg_id, result, error);
  }

  void crop_bytes(size_t size, size_t offset){
    decoder.consume(size, offset);
  }

  void pop_first() {
    uint8_t temp_buffer[512];
    decoder.pop_packet(temp_buffer, 512);
  }

  void print_raw_buf(){

    Serial.print("Decoder raw buffer content: ");

    for (size_t i = 0; i < decoder._bytes_stored; i++) {

        Serial.print(decoder._raw_buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");
  }

};

#endif // RPCLITE_DECODER_TESTER_H