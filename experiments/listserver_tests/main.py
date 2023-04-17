import socket, random, time

xo = bytearray(open("server.txt", "rb").read())

while True:
    x = xo.copy()
    x[0x23] = random.randint(1, 32)
    x[0x24] = 0x32

    for i in range(0x2, 0x1F, 1):
        x[i] = ord("|")
        x[i + 1] = ord(chr(i + random.randint(1, 4))) ^ random.randint(0, 255)

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("localhost", 10054))
    sock.send(x)
    time.sleep(1)
    sock.send(b"\x00")
    sock.close()

    print("killed")

    time.sleep(0)
