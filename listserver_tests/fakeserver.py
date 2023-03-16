import socket, threading

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

fakePing = bytes(udpchecksum(bytearray(b"\x06\x00\x00\x00\x00\x0024\x00\x00\x01\x00\x00\x32\x01s\x00\x00")))

def main():
    HOST = "localhost"
    PORT = 10052

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    sock.bind((HOST, PORT))

    print(f"Server started on port {PORT}")

    while True:
        data, addr = sock.recvfrom(2048)

        print(f"Received {len(data)} bytes from {addr}, probably a ping")
        print(data)

        sock.sendto(fakePing, addr)

threading.Thread(target=main).start()