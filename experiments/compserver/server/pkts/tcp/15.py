from shared import *
import random
import socket

import zlib

def calculate_crc(data: bytes) -> int:
    return zlib.crc32(data)

# connect request
def pkt(data, addr, tcp: socket.socket, udp):
    pkt = create_tcp_packet([
        0x10,
        0x1, # socket id
        0x1, # no use
        create_jj2_string("empty.j2l"),
        bytearray((calculate_crc(open("./data/empty.j2l", "rb").read())).to_bytes(4)), # level crc
        bytearray((calculate_crc(open("./data/castle1.j2t", "rb").read())).to_bytes(4)), # tileset crc
        0x1, # gamemode
        0x1, # max score
        # plus
        0x00, 0x00, 0x00, 0x00, # some checksum
        0x00, 0x00, 0x00, 0x00, # random each time, possibly seed?
        0x00, 0x05, 0x00, 0x12, #0x00, # plus version
        0x00, 0x00, 0x00, 0x00
    ])
    tcp.send(pkt)
