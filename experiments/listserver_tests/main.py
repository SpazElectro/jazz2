import socket, random, time

xo = bytearray(open("server.txt", "rb").read())

x = xo.copy()
x[0x23] = random.randint(1, 32)
x[0x24] = 0x32
x[0x2] = ord("W")
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("localhost", 10054))
sock.send(x)
