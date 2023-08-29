import socket, os, enum

# packetarraytype
PAT_TCP = 0
PAT_UDP = 1
PAT_DETERMINE = 2

global_server_addr = None
global_tcp_socket  = None
global_udp_socket  = None

def set_global_server_addr(new_global_server_addr):
    global global_server_addr
    global_server_addr = new_global_server_addr

def set_global_sockets(new_global_tcp, new_global_udp):
    global global_tcp_socket, global_udp_socket
    global_tcp_socket = new_global_tcp
    global_udp_socket = new_global_udp

def Packet(packetLengthA=0x0, packetLengthB=0x0, packetIdA=0x0, packetIdB=0x0):
    return bytes([packetLengthA, packetLengthB + 0x3, packetIdA, packetIdB])

def ChatMessage(sockId: int, msg: str):
    pk = Packet(packetLengthB=len(msg), packetIdB=0x1B)
    pk += (bytes([sockId, 0x00]) + msg.encode())
    return pk

def determine_pat_type(file_name):
    if file_name.startswith("tcp"):
        return PAT_TCP
    elif file_name.startswith("udp"):
        return PAT_UDP
    else:
        raise ValueError("Unknown packet type")

def send_packet_array(name: str, pat_type=2):
    files = os.listdir(f"./packetarrays/{name}")
    sorted_files = sorted(files, key=lambda x: int(x.split(".")[0][3:]))

    for item in sorted_files:
        packet_type = determine_pat_type(item) if pat_type == 2 else pat_type
        packet = open(f"./packetarrays/{name}/{item}", "rb").read()

        if packet_type == PAT_TCP:
            global_tcp_socket.sendall(packet)  # type: ignore
        elif packet_type == PAT_UDP:
            global global_server_addr
            global_udp_socket.sendto(packet, global_server_addr)  # type: ignore

from construct import Byte

# https://github.com/bswck/jj2_old/blob/master/jj2/protocols/game.py#L477-L484
# def checksum(serialized):
#     arr = bytes((79, 79)) + serialized
#     lsb = msb = 1
#     for i in range(2, len(arr)):
#         lsb += arr[i]
#         msb += lsb
#     return Byte.build(lsb % 251) + Byte.build(msb % 251)
def checksum(data):
    x = 1
    y = 1
    for byte in data:
        x += byte
        y += x
    return bytes([x & 0xFF, y & 0xFF])  # Use bitwise AND to limit to 8 bits

# [ 04 01 00 00 00 00 80 ]'s checksum should be [ 00 07 ]
# print(checksum(bytes(bytearray([0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80]))))
# print(bytes(bytearray([0x00, 0x07])))

disconnect_messages = {
    0x01: "Server full",
    0x02: "Version different",
    0x03: "?",
    0x04: "Error during handshaking",
    0x05: "Feature not supported in shareware",
    0x06: "Error downloading level",
    0x07: "Connection lost",
    0x08: "Winsock error",
    0x09: "Connection timed out",
    0x0A: "Server stopped",
    0x0B: "Kicked off",
    0x0C: "Banned"
}

def parse_disconnect_message(value):
    return disconnect_messages.get(value, "Unknown error")
