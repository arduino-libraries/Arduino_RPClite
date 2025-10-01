/*
 * Copyright (c) 2020 Arduino.  All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STREAM_MOCK_H_
#define STREAM_MOCK_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <deque>
#include <api/Stream.h>

class StreamMock : public arduino::Stream
{
public:
  void operator << (char const * str) {
    for (size_t c = 0; c < strlen(str); c++) {
      _readStream.push_back(str[c]);
    }
  }

  void push_array(const unsigned char *str, size_t len) {
    for (size_t c = 0; c < len; c++) {
      _readStream.push_back(static_cast<char>(str[c]));
    }
  }

  int pull_available() {
    return _writeStream.size();
  }

  int pull_array(unsigned char *str, size_t len) {
    if (_writeStream.size() < len) {
      return 0; // Not enough data to read
    }
    for (size_t c = 0; c < len; c++) {
      str[c] = _writeStream.front();
      _writeStream.pop_front();
    }
    return len;
  }

private:
  std::deque<unsigned char> _readStream;
  std::deque<unsigned char> _writeStream;

public:
  // --------------------------------
  // Implementation of Stream methods
  // --------------------------------

  virtual size_t write(uint8_t ch) override {
    _writeStream.push_back(static_cast<char>(ch));
    return 1;
  }

  virtual int available() override {
    return _readStream.size();
  }

  virtual int read() override {
    if (available() == 0) {
      return -1;
    }
    unsigned char c = _readStream.at(0);
    _readStream.pop_front();
    return c;
  }

  virtual int peek() override {
    if (available() == 0) {
      return -1;
    }
    return _readStream.at(0);
  }
};

#endif /* STREAM_MOCK_H_ */
