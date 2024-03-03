import socket
import threading
import time
import signal
import sys
import hexdump

HOST = ("127.0.0.1", 10052)
LIST_PACKET = bytearray([0x44, 0x27, 0x74, 0x6F, 0x6F, 0x20, 0x6C, 0x61, 0x74, 0x65, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x74, 0x68, 0x69, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x2E, 0x4E, 0x32, 0x32, 0x82, 0x32, 0x34, 0x20, 0x20])

def udpchecksum(bytes):
    """"
    Prepend UDP checksum
    
    JJ2 expects UDP datagrams to be perceded by a two-byte checksum - this method adds that checksum
    Thanks to DJazz for the PHP reference implementation!
    
    :param bytes: Bytearray to checksum
    :return: Bytearray with checksum
    """
    x = 79
    y = 79
    byte_count = len(bytes)
    for i in range(2, byte_count):
        x += bytes[i]
        y += x

    bytes[0] = x % 251
    bytes[1] = y % 251

    return bytes

def listworker():
    x = LIST_PACKET.copy()
    x[0x23] = 1
    x[0x24] = 0x32
    input_bytes = "WALAD".encode("utf-8")[:32]
    LIST_PACKET[0x2:0x2 + len(input_bytes)] = input_bytes

    # sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # sock.connect(("localhost", 10054))
    # sock.send(x)

def udpworker():
    log = lambda _: print(f"[UDP] {_}")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(HOST)
    log("open")

    while True:
        data, addr = sock.recvfrom(2048)
        log(addr)
        hexdump.hexdump(data)

        # sock.sendto(bytearray([0x88, 0xAC, 0x04, 0x01, 00, 00, 00, 00, 0x82 ]), addr)
        messageId = data[2]

        if messageId == 0x05:
            sock.sendto(bytearray.fromhex("98 A8 06 00 09 00 00 00 32 34 20 20 01 01 00 10 04 74 65 73 74 00 02 00 "), addr)
        elif messageId == 0x03:
            sock.sendto(bytearray.fromhex("D9 45 04 00 52 F2 0B 00 80 "), addr)
        elif messageId == 0x09:
            sock.sendto(bytearray.fromhex("08 3F 20 03 0C 00 05 00 "), addr)
            sock.sendto(bytearray.fromhex("1D 0E 01 01 01 01 10 18 20 28 11 01 0A 0D 00 00 53 70 61 7A 20 45 6C 65 63 74 72 6F 00 "), addr)
            sock.sendto(bytearray.fromhex("0B 3F 0C 00 05 00 00 00 00 81 01 "), addr)
def tcpworker():
    log = lambda _: print(f"[TCP] {_}")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(HOST)
    sock.listen(1)
    log("open")
    
    while True:
        client, addr = sock.accept()
        log(addr)
        data = client.recv(2048)
        hexdump.hexdump(data)

        messageId = data[2]

        if messageId == 0xC0:
            client.send(bytearray.fromhex("31 10 01 01 0B 63 61 73 74 6C 65 31 2E 6A 32 6C 2A 19 50 07 5F 97 89 FB 00 0A BC A9 03 7C 41 D8 9A 41 0C 00 05 00 01 64 D0 FF 6F 00 28 61 EA 0F 01 "))
def main():
    signal.signal(signal.SIGINT, lambda signum, frame: sys.exit(0))

    lt = threading.Thread(target=listworker)
    lt.daemon = True
    lt.start()
    ut = threading.Thread(target=udpworker)
    ut.daemon = True
    ut.start()
    tt = threading.Thread(target=tcpworker)
    tt.daemon = True
    tt.start()

    while True:
        time.sleep(1)

if __name__ == "__main__":
    main()
