import socket, time

def hexstr_to_bytes(hexstr):
    hex_list = hexstr.split()
    byte_list = [int(hex_digit, 16) for hex_digit in hex_list]
    return bytes(byte_list)

sock = socket.socket()
sock.connect(("localhost", 10052))
sock.send(hexstr_to_bytes("06 0D 05 00"))

print("sent")

data = sock.recv(1024)

with open(f"client/packets/udp/{time.time()}", "wb") as f:
    f.write(data)

print("got")