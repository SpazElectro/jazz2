from shared import *
import random

def pkt(data, addr, sock):
    number_in_list = data[3]
    
    pkt = create_udp_packet([
        0x06, # pkt id
        number_in_list, # number in list
        0xF3,
        0x00, 0x00, 0x00,
        bytearray(b"sexy"),
        0x1,
        69, # player count
        0x0, # game mode
        128, # max players
        create_jj2_string("JazzJackrabbit Sex Mod Playtest"), # server name
        0x2, 0x1
    ])

    sock.sendto(pkt, addr)