def add(a, b):
    return a + b

def greet(name):
    return f"Hello, {name}!"

server = SerialServer(port='/dev/ttyUSB0', baudrate=115200)
server.register_callback('add', add)
server.register_callback('greet', greet)
server.start()

print("Serial server running... Press Ctrl+C to exit.")
try:
    while True:
        pass  # Keep main thread alive
except KeyboardInterrupt:
    server.stop()
    print("\nSerial server stopped.")