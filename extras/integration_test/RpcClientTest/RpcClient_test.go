package RpcClientZeroTest

import (
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"os"
	"testing"
	"time"

	"github.com/arduino/go-paths-helper"
	"github.com/stretchr/testify/require"
	"github.com/vmihailenco/msgpack/v5"
	"go.bug.st/serial"
)

func TestBasicComm(t *testing.T) {
	// Get the upload port to upload the sketch
	fqbn, _, uploadPort := getFQBNAndPorts(t)
	{
		// Upload the sketch
		cli, err := paths.NewProcess(nil, "arduino-cli", "compile", "--fqbn", fqbn, "--library", "../../..", "-u", "-p", uploadPort)
		require.NoError(t, err)
		cli.RedirectStderrTo(os.Stderr)
		cli.RedirectStdoutTo(os.Stdout)
		require.NoError(t, cli.Run())
	}

	// Get the rpc and debug ports
	fqbn2, rpcPort, debugPort := getFQBNAndPorts(t)
	require.Equal(t, fqbn, fqbn2, "FQBN mismatch between upload and run ports: %s != %s", fqbn, fqbn2)

	// Connect to the RPC serial port
	_rpcSer, err := serial.Open(rpcPort, &serial.Mode{BaudRate: 115200})
	rpcSer := &DebugStream{upstream: _rpcSer, portname: rpcPort}
	require.NoError(t, err)
	t.Cleanup(func() { rpcSer.Close() })
	in := msgpack.NewDecoder(rpcSer)
	out := msgpack.NewEncoder(rpcSer)
	out.UseCompactInts(true)

	// Connect to the Debug serial port
	debugSer, err := serial.Open(debugPort, &serial.Mode{BaudRate: 115200})
	require.NoError(t, err)
	t.Cleanup(func() { debugSer.Close() })
	expectDebug := func(exp string) {
		buff := make([]byte, len(exp))
		read := 0
		for read < len(exp) {
			n, err := debugSer.Read(buff[read:])
			read += n
			require.NoError(t, err)
		}
		require.Equal(t, exp, string(buff))
	}

	// Timeout fallback: close the connection after 10 seconds, if the test do not go through
	go func() {
		time.Sleep(10 * time.Second)
		rpcSer.Close()
		debugSer.Close()
	}()

	// 1: Receive an RPC call to the "mult" method with 2 arguments
	// and send back the result
	t.Run("RPCClientCallFloatArgs", func(t *testing.T) {
		arr, err := in.DecodeSlice()
		require.NoError(t, err)
		require.Equal(t, []any{int8(0), int8(1), "mult", []any{2.0, 3.0}}, arr)
		err = out.Encode([]any{1, 1, nil, 6.0})
		require.NoError(t, err)
		expectDebug("mult(2.0, 3.0)\r\n")
		expectDebug("-> 6.00\r\n")
	})

	// 2: Receive an RPC call to the "mult" method with 1 argument (wrong number of arguments)
	// and send back an error with [int, string] format
	t.Run("RPCClientCallFloatArgsError", func(t *testing.T) {
		arr, err := in.DecodeSlice()
		require.NoError(t, err)
		require.Equal(t, []any{int8(0), int8(2), "mult", []any{2.0}}, arr)
		err = out.Encode([]any{1, 2, []any{1, "missing parameter"}, nil})
		require.NoError(t, err)
		expectDebug("mult(2.0)\r\n")
		expectDebug("-> error\r\n")
	})

	// 3, 4: Receive an RPC call to the "or" method with 1 or 2 arguments
	// and send back the result
	t.Run("RPCClientCallBoolArgs", func(t *testing.T) {
		arr, err := in.DecodeSlice()
		require.NoError(t, err)
		require.Equal(t, []any{int8(0), int8(3), "or", []any{true, false}}, arr)
		err = out.Encode([]any{1, 3, nil, true})
		require.NoError(t, err)
		expectDebug("or(true, false)\r\n")
		expectDebug("-> true\r\n")

		arr, err = in.DecodeSlice()
		require.NoError(t, err)
		require.Equal(t, []any{int8(0), int8(4), "or", []any{false}}, arr)
		err = out.Encode([]any{1, 4, nil, false})
		require.NoError(t, err)
		expectDebug("or(false)\r\n")
		expectDebug("-> false\r\n")
	})

	// 5: Receive an RPC call to the "mult" method with 1 argument (wrong number of arguments)
	// and send back an error with [int, string] format with a long string
	t.Run("RPCClientCallFloatArgsErrorWithLongString", func(t *testing.T) {
		arr, err := in.DecodeSlice()
		require.NoError(t, err)
		require.Equal(t, []any{int8(0), int8(5), "mult", []any{2.0}}, arr)
		err = out.Encode([]any{1, 5, []any{2, "method get_led_state not available"}, nil})
		require.NoError(t, err)
		expectDebug("mult(2.0)\r\n")
		expectDebug("-> error\r\n")
	})

	// RPC: Receive an RPC call to the "mult" method with 1 argument (wrong number of arguments)
	// and send back a custom error without [int, string] format
	// t.Run("RPCClientCallFloatArgsErrorCustom", func(t *testing.T) {
	// 	arr, err := in.DecodeSlice()
	// 	require.NoError(t, err)
	// 	require.Equal(t, []any{int8(0), int8(3), "mult", []any{2.0}}, arr)
	// 	err = out.Encode([]any{1, 3, "missing parameter", nil})
	// 	require.NoError(t, err)
	// 	expectDebug("mult(2.0)\r\n")
	// 	expectDebug("-> error\r\n")
	// })
}

func getFQBNAndPorts(t *testing.T) (fqbn string, rpcPort string, uploadPort string) {
	cli, err := paths.NewProcess(nil, "arduino-cli", "board", "list", "--json")
	require.NoError(t, err)
	out, _, err := cli.RunAndCaptureOutput(t.Context())
	require.NoError(t, err)
	var cliResult struct {
		DetectedPorts []struct {
			MatchingBoards []struct {
				Fqbn string `json:"fqbn"`
			} `json:"matching_boards"`
			Port struct {
				Address    string `json:"address"`
				Properties struct {
					Vid string `json:"vid"`
					Pid string `json:"pid"`
				} `json:"properties"`
			} `json:"port"`
		} `json:"detected_ports"`
	}
	require.NoError(t, json.Unmarshal(out, &cliResult))
	checkFQBN := func(boardFQBN string) {
		if fqbn != boardFQBN {
			fqbn = boardFQBN
			uploadPort = ""
			rpcPort = ""
		}
	}
	for _, port := range cliResult.DetectedPorts {
		for _, board := range port.MatchingBoards {
			if board.Fqbn == "arduino:mbed_giga:giga" {
				checkFQBN(board.Fqbn)
				uploadPort = port.Port.Address
			}
			if board.Fqbn == "arduino:samd:arduino_zero_edbg" {
				checkFQBN("arduino:samd:arduino_zero_native")
				rpcPort = port.Port.Address
			}
			if board.Fqbn == "arduino:samd:arduino_zero_native" {
				checkFQBN(board.Fqbn)
				uploadPort = port.Port.Address
			}
			if board.Fqbn == "arduino:mbed_nano:nanorp2040connect" {
				checkFQBN(board.Fqbn)
				uploadPort = port.Port.Address
			}
		}
	}
	if rpcPort == "" {
		for _, port := range cliResult.DetectedPorts {
			if port.Port.Properties.Vid == "0x0483" && port.Port.Properties.Pid == "0x374B" {
				rpcPort = port.Port.Address
			}
			if port.Port.Properties.Vid == "0x1A86" && port.Port.Properties.Pid == "0x55D4" {
				rpcPort = port.Port.Address
			}
		}
	}
	require.NotEmpty(t, uploadPort, "Upload port not found")
	require.NotEmpty(t, rpcPort, "Debug port not found")
	return fqbn, rpcPort, uploadPort
}

// DebugStream is a wrapper around io.ReadWriteCloser that logs the data
// read and written to the stream in hex format.
// It is used to debug the communication with the Arduino board.
type DebugStream struct {
	upstream io.ReadWriteCloser
	portname string
}

func (d *DebugStream) Read(p []byte) (n int, err error) {
	n, err = d.upstream.Read(p)
	if err != nil {
		fmt.Printf("%s READ  ERROR: %v\n", d.portname, err)
	} else {
		fmt.Printf("%s READ  << %s\n", d.portname, hex.EncodeToString(p[:n]))
	}
	return n, err
}

func (d *DebugStream) Write(p []byte) (n int, err error) {
	n, err = d.upstream.Write(p)
	if err != nil {
		fmt.Printf("%s WRITE ERROR: %v\n", d.portname, err)
	} else {
		fmt.Printf("%s WRITE >> %s\n", d.portname, hex.EncodeToString(p[:n]))
	}
	return n, err
}

func (d *DebugStream) Close() error {
	err := d.upstream.Close()
	fmt.Printf("%s CLOSE", d.portname)
	if err != nil {
		fmt.Printf(" (ERROR: %v)", err)
	}
	fmt.Println()
	return err
}
