/*
 * Copyright (c) 2025 Arduino.  All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch.hpp>
#include <StreamMock.h>
#include <Arduino_RPClite.h>
#include "utils.h"

TEST_CASE("RPCClient::call", "[RPCClient-01]")
{
  WHEN("Make an RPC call")
  {
    StreamMock mock;
    SerialTransport transport(&mock);
    RPCClient rpc(transport);
    byte response[] = {
      ARRAY_N(4),
        RESP_TAG,
        0,
        NIL,
        FLOAT_32, 0x40, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // 6.0
    };
    mock.push_array(response, sizeof(response));

    float result = 0.0f;
    bool ok = rpc.call("mult", result, 2.0, 3.0);
    REQUIRE(ok == true);
    REQUIRE(result == 6.0f);
    byte expected[] = {
      ARRAY_N(4),
        CALL_TAG,
        0, // msg_id
        STRING_N(4), 'm', 'u', 'l', 't',
        ARRAY_N(2), // args
          FLOAT_32, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 2.0
          FLOAT_32, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 3.0
    };
    REQUIRE(mock.pull_available() == sizeof(expected));
    unsigned char got[sizeof(expected)];
    REQUIRE(mock.pull_array(got, sizeof(expected)) == sizeof(expected));
    COMPARE_ARRAYS(expected, got);
  }

  WHEN("Make an RPC call with an missing parameter, followed by a response without an error")
  {
    StreamMock mock;
    SerialTransport transport(&mock);
    RPCClient rpc(transport);
    {
      const unsigned char response[] = {
        ARRAY_N(4),
          RESP_TAG,
          1,
          ARRAY_N(2),
            1,
            STRING_N(17), 'm', 'i', 's', 's', 'i', 'n', 'g', ' ', 'p', 'a', 'r', 'a', 'm', 'e', 't', 'e', 'r',
          NIL,
      };
      mock.push_array(response, sizeof(response));

      float result = 0.0f;
      bool ok = rpc.call("mult", result, 2.0);
      REQUIRE(ok == false);
      REQUIRE(result == 0.0f);
      const unsigned char expected[] = {
        ARRAY_N(4),
          CALL_TAG,
          1, // msg_id
          STRING_N(4), 'm', 'u', 'l', 't',
          ARRAY_N(1), // args
            FLOAT_32, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 2.0
      };
      REQUIRE(mock.pull_available() == sizeof(expected));
      unsigned char got[sizeof(expected)];
      REQUIRE(mock.pull_array(got, sizeof(expected)) == sizeof(expected));
      COMPARE_ARRAYS(expected, got);
      REQUIRE(rpc.lastError.code == 1);
      REQUIRE(rpc.lastError.traceback == "missing parameter");
    }
    // Make another call without error
    {
      byte response[] = {
        ARRAY_N(4),
          RESP_TAG,
          2,
          NIL,
          FLOAT_32, 0x40, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // 6.0
      };
      mock.push_array(response, sizeof(response));

      float result = 0.0f;
      bool ok = rpc.call("mult", result, 2.0, 3.0);
      REQUIRE(ok == true);
      REQUIRE(result == 6.0f);
      byte expected[] = {
        ARRAY_N(4),
          CALL_TAG,
          2, // msg_id
          STRING_N(4), 'm', 'u', 'l', 't',
          ARRAY_N(2), // args
            FLOAT_32, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 2.0
            FLOAT_32, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 3.0
      };
      REQUIRE(mock.pull_available() == sizeof(expected));
      unsigned char got[sizeof(expected)];
      REQUIRE(mock.pull_array(got, sizeof(expected)) == sizeof(expected));
      COMPARE_ARRAYS(expected, got);
      REQUIRE(rpc.lastError.code == 0);
      REQUIRE(rpc.lastError.traceback == "");
    }
  }
}
