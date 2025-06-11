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

