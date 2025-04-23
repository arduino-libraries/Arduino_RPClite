from serial_server import SerialServer

def add(a, b):
    return a + b

def greet(name):
    return f"Hello, {name}!"

def multiply(a, b):
    return a*b

def divide(a, b):
    return a/b

PORT = '/dev/ttySTM0'

server = SerialServer(port=PORT, baudrate=115200)
server.register_callback('add', add)
server.register_callback('mult', multiply)
server.register_callback('divi', divide)
server.register_callback('greet', greet)
server.start()

print("Serial server running... Press Ctrl+C to exit.")
try:
    while True:
        pass  # Keep main thread alive
except KeyboardInterrupt:
    server.stop()
    print("\nSerial server stopped.")