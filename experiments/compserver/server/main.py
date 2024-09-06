import sys
from shared import *
import random
import socket
import os

udp_server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
udp_server.bind(("0.0.0.0", 10052))


while True:
    data, addr = udp_server.recvfrom(2048)
    checksum = data[0:2]
    expected_checksum = calculate_checksum(bytearray(data[2:]))
    if bytearray(checksum) != expected_checksum:
        print("Checksums do not match!")
        print(f"Got {checksum} VS Expected {expected_checksum}")
        continue
    
    packet_id = data[2]
    if packet_id == 0x05:
        # number_in_list = data[3]
        # testpackets/*.bin
        import importlib
        importlib.import_module("pkts.5").pkt(data, addr, udp_server)
        #del sys.modules["pkts.5.shared"]
        del sys.modules["pkts.5"]
        # pkt = create_udp_packet([
        #     0x06, # pkt id
        #     number_in_list, # number in list
        #     0xF3,
        #     0x00,
        #     0x00, 0x00,
        #     bytearray(b"24  "),
        #     0x1,
        #     random.randint(0, 255), # player count
        #     0x2, # game mode
        #     random.randint(0, 255), # max players
        #     create_jj2_string("test"), # server name
        #     0x2, 0x0
        # ])
        # udp_server.sendto(pkt, addr)
        # print("Sent!")
        # print(pkt)
    elif packet_id == 0x3:
        pkt = create_udp_packet([
            0x4,
            0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00 # is passwordded?
        ])
        udp_server.sendto(pkt, addr)
        print("Update!")
    else:
        print(packet_id)