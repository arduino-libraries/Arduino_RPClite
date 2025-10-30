## Unit test for the Arduino_RPClite library

This directory contains unit-tests that can run locally on PC.
The transport is mocked, no external hardware is required to run the tests.

### Running the test

Prerequisites:

* `cmake`
* A working C++ compiler.

From a terminal, change directory to this folder, and run:

```
mkdir build
cd build
cmake ..
make
```

it should compile the test and output the executable binary in `build/bin/test-Arduino_RPClite`.

The output should look similar to the following:

```
$ bin/test-Arduino_RPClite
===============================================================================
All tests passed (91 assertions in 1 test case)
```
