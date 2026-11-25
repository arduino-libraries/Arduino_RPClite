#pragma once

#include <string>

// Poor-man MsgPack encoder
#define ARRAY_N(x) (0x90 + (x))
#define STRING_N(x) (0xA0 + (x))
#define NIL 0xc0
#define FLOAT_32 0xcb
#define CALL_TAG 0x00
#define RESP_TAG 0x01

#define COMPARE_ARRAYS(expected, got) \
  REQUIRE(to_hex_string(expected, sizeof(expected)) == to_hex_string(got, sizeof(expected)))

std::string to_hex_string(const unsigned char* data, size_t size);
