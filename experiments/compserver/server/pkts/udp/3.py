from shared import *
import random

# main menu 2
def pkt(data, addr, tcp, udp):
    numberInList = data[3]
    ping = data[4:10]

    pkt = create_udp_packet([
        0x4,
        numberInList, # number in list
        ping[0], ping[1], ping[2], ping[3], # ping
        0x00 # flags
    ])
    udp.sendto(pkt, addr)
