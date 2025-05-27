# SPDX-License-Identifier: LGPL-2.1-or-later

##########################################################################

cmake_minimum_required(VERSION 2.8)

##########################################################################

project(test-Arduino_RPClite)

##########################################################################

include_directories(../../src)
include_directories(ArduinoCore-api)
include_directories(include)
include_directories(external/catch/v2.13.9/include)
include_directories(external/DebugLog)
include_directories(external/ArxTypeTraits)
include_directories(external/ArxContainer)

##########################################################################

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

##########################################################################

set(TEST_TARGET ${CMAKE_PROJECT_NAME})

##########################################################################

set(TEST_SRCS
  src/RPC/test_RPCClient.cpp
  src/Print/test_clearWriteError.cpp
  src/Print/test_getWriteError.cpp
  src/Print/test_print.cpp
  src/Print/test_println.cpp
  src/Print/test_availableForWrite.cpp
  src/Stream/test_find.cpp
  src/Stream/test_findUntil.cpp
  src/Stream/test_getTimeout.cpp
  src/Stream/test_parseFloat.cpp
  src/Stream/test_parseInt.cpp
  src/Stream/test_readBytes.cpp
  src/Stream/test_readBytesUntil.cpp
  src/Stream/test_readString.cpp
  src/Stream/test_readStringUntil.cpp
  src/Stream/test_setTimeout.cpp
  src/String/test_concat.cpp
  src/String/test_operators.cpp
  src/String/test_compareTo.cpp
  src/String/test_comparisonFunc.cpp
  src/String/test_characterAccessFunc.cpp
  src/String/test_substring.cpp
  src/String/test_indexOf.cpp
  src/String/test_lastIndexOf.cpp
  src/String/test_length.cpp
  src/String/test_move.cpp
  src/String/test_remove.cpp
  src/String/test_replace.cpp
  src/String/test_String.cpp
  src/String/test_toDouble.cpp
  src/String/test_toFloat.cpp
  src/String/test_toInt.cpp
  src/String/test_toLowerCase.cpp
  src/String/test_toUpperCase.cpp
  src/String/test_trim.cpp
)

set(TEST_DUT_SRCS
  ArduinoCore-api/api/Common.cpp
  ArduinoCore-api/api/String.cpp
  ArduinoCore-api/api/Stream.cpp
  ArduinoCore-api/api/Print.cpp
)

##########################################################################

set(TEST_TARGET_SRCS
  src/main.cpp
  src/delay.c
  src/dtostrf.cpp
  src/itoa.cpp
  src/MillisFake.cpp
  src/PrintMock.cpp
  src/StreamMock.cpp
  ${TEST_SRCS}
  ${TEST_DUT_SRCS}
)

##########################################################################

add_compile_definitions(HOST)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(-Wno-cast-function-type)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -Wno-deprecated-copy")

##########################################################################

add_executable(
  ${TEST_TARGET}
  ${TEST_TARGET_SRCS}
)

##########################################################################

