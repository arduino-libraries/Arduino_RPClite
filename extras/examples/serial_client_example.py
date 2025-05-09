from serial_client import SerialClient

PORT = '/dev/ttySTM0'

client = SerialClient(port=PORT, baudrate=115200)

client.call("add", 15, 7)