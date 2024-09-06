import random
import socket

def generate_flags(mode: int, private: bool, plusonly: bool) -> int:
    flags = mode << 1
    if private:
        flags |= 1
    else:
        flags &= ~1
    if plusonly:
        flags |= 128
    else:
        flags &= ~128
    return flags

def create_server_data(server_name: str, port: int, player_count: int, max_players: int, version: str, gamemode: int, private: bool, plusonly: bool):
    data = b""
    data += port.to_bytes(3, "little")
    data += server_name.ljust(32, "\x00").encode()
    data += player_count.to_bytes()
    data += max_players.to_bytes()
    data += generate_flags(gamemode, private, plusonly).to_bytes()
    data += version.encode()
    return data

import string
def generate_random_bytes(length):
    random_string = ''.join(random.choice(string.ascii_letters) for _ in range(length))
    return random_string.encode('ascii')


def create_server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("localhost", 10054))

    player_count = 21
    sock.send(create_server_data(
        "test", 10052, player_count, 7, "24  ", 2, True, True
    ))

    import time
    time.sleep(3)
    TIMEOUT = 0.5
    while True:
        # data = sock.recv(1024)
        sock.send(b"\x00"+((random.randint(1, 32)).to_bytes()))
        time.sleep(TIMEOUT)
        sock.send(b"\x01"+((random.randint(1, 15)).to_bytes()))
        time.sleep(TIMEOUT)
        sock.send(b"\x02"+generate_random_bytes(32))
        time.sleep(TIMEOUT)
        sock.send(b"\x03"+((random.randint(1, 32)).to_bytes()))
        time.sleep(1)
        # sock.send(b"\x02\x02"+("hello".ljust(33, "\x00").encode()))

create_server()