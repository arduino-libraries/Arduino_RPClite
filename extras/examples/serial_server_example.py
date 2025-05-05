import random
from serial_server import SerialServer

def add(a, b):
    return a + b

def greet(name):
    return f"Hello, {name}!"

def multiply(a, b):
    return a*b

def divide(a, b):
    if b == 0:
        print("0 denominator: this will raise an exception")
    return a/b

def blink():
    """
    Used for notification
    """
    print("Blink maybe")

def get_rand():
    """
    Used for testing parameter-less RPCs
    """
    r = random.randint(0, 10)
    print(f"returning a random integer: {r}")
    return r


PORT = '/dev/ttySTM0'

server = SerialServer(port=PORT, baudrate=115200)
server.register_callback('add', add)
server.register_callback('mult', multiply)
server.register_callback('divi', divide)
server.register_callback('greet', greet)
server.register_callback('blink', blink)
server.register_callback('get_rand', get_rand)

server.start()

print("Serial server running... Press Ctrl+C to exit.")
try:
    while True:
        pass  # Keep main thread alive
except KeyboardInterrupt:
    server.stop()
    print("\nSerial server stopped.")