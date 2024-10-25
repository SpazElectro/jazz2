from shared import *
import random

# main menu 1
def pkt(data, addr, tcp, udp):
    number_in_list = data[3]
    
    pkt = create_udp_packet([
        0x06, # pkt id
        number_in_list, # number in list
        0xF3,
        0x00, 0x00, 0x00,
        bytearray(b"24  "),
        0x1,
        1, # player count
        0x0, # game mode
        32, # max players
        create_jj2_string("Dedicated Server"), # server name
        0x2, 0x1
    ])

    udp.sendto(pkt, addr)