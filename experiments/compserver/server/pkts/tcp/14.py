from shared import *
import random
import socket

# TCP
# 10  26.195.94.18:10052  26.195.94.18:47094  11  Recv  
# 0000  0B 3F 0C 00 05 00 00 00 00 81 01                   .?.........

# TCP
# 11  26.195.94.18:10052  26.195.94.18:47094  80  Recv  
# 0000  04 5B 00 00 3C 12 02 00 00 01 00 20 20 20 28 11    .[..<......   (.
# 0010  00 0A 0D 00 00 7C 7C 53 70 61 7A 20 7C 4E 69 74    .....||Spaz |Nit
# 0020  72 6F 00 01 01 01 01 20 20 20 28 11 01 0A 0D 00    ro.....   (.....
# 0030  00 7C 7C 53 70 61 7A 20 7C 4E 69 74 72 6F 32 00    .||Spaz |Nitro2.
# 0040  02 13 08 49 00 FF FF 01 09 00 06 41 01 00 01 FF    ...I.......A....

# UDP
# 12  :0  0.0.0.0:51161  6  RecvFrom  
# 0000  02 00 00 38                                  ;E...8

# joining details
def pkt(data, addr, tcp: socket.socket, udp: socket.socket):
    pass
    # print(data)
    # pkt = create_tcp_packet([
    #     0x3F,
    #     0x0C, 0x00,
    #     0x05,
    #     0x00, 0x00, 0x00, 0x00,
    #     0x81, 0x01
    # ])
    # pkt2 = create_tcp_packet([
    #     bytearray(bytes.fromhex("5B 00 00 3C 12 02 00 00 01 00 20 20 20 28 11")),
    #     bytearray(bytes.fromhex("00 0A 0D 00 00 7C 7C 53 70 61 7A 20 7C 4E 69 74")),
    #     bytearray(bytes.fromhex("72 6F 00 01 01 01 01 20 20 20 28 11 01 0A 0D 00")),
    #     bytearray(bytes.fromhex("00 7C 7C 53 70 61 7A 20 7C 4E 69 74 72 6F 32 00")),
    #     bytearray(bytes.fromhex("02 13 08 49 00 FF FF 01 09 00 06 41 01 00 01 FF")),
    # ])
    # tcp.send(pkt)
    # tcp.send(pkt2)
    # tcp.send(create_tcp_packet([0x51]))