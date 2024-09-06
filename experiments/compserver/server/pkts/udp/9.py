from shared import *
import socket
import random

def pkt(data, addr, tcp: socket.socket, udp: socket.socket):
    print(data)
    if data[3] == 0:
        print("09 => 0")
        # pkt = create_udp_packet([

        # ])
        pkt3 = create_udp_packet([
            0x02, 0x00, 0x00, 0x38
        ])
        udp.sendto(pkt3, addr)
    # print("Sent some random shit")
    # print(pkt3)

    # 12 UDP Got unknown_02
    # 13 UDP Sent unknown_09
    # 14 UDP Sent unknown_09
    # 15 TCP Got unknown_51
    # 16 TCP Sent unknown_1A
    # 17 TCP Got unknown_44
    # 18 TCP Got unknown_41
