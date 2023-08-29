from packet import *
import socket, time

import hexdump

server_host = "127.0.0.1"
server_port = 10052
server_address = (server_host, server_port)
set_global_server_addr(server_address)

tcp_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcp_client.connect(server_address)

udp_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

class FileUpdateHandler(FileSystemEventHandler):
    def reload(self, event):
        if event.src_path.endswith("jazz2.log"):
            return
        if ".git" in event.src_path:
            return
        
        print(event.src_path)
        # tcp_client.send(ChatMessage(1, "/r"))

    def on_created(self, event):
        self.reload(event)

    def on_deleted(self, event):
        self.reload(event)

    def on_modified(self, event):
        self.reload(event)


folder_path = "../../"

event_handler = FileUpdateHandler()
observer = Observer()
observer.schedule(event_handler, folder_path, recursive=True)
observer.start()

def string_to_hex_bytes(input_string):
    hex_values = input_string.split()
    hex_bytes = bytes.fromhex(''.join(hex_values))
    return hex_bytes

set_global_sockets(tcp_client, udp_client)
# send_packet_array("login")

# struct joinRequest {
# 	byte 0x09;		// packetLength, always 9 here
# 	byte 0x0F;
# 	unsigned short UDPbind;	// Alternatively "byte UDPbind[2]", see below
# 	char clientVersion[4];	// Common values: "21  " for 1.23, "24  " for 1.24
# 	byte numberOfPlayersFromClient;
# }

# No idea how to generate "UDPbind", so, 0x0A and 0xD1
tcp_client.sendall(bytes(bytearray([
    0x09, 0x0F, 0x0A, 0xD1, ord("2"), ord("4"), ord(" "), ord(" "), 0x1
])))

tcp_client.sendall(bytes(bytearray([
    0x08, 0x3F, 0x20, 0x03, 0x0C, 0x00, 0x05, 0x00
])))

# struct joinDetails {
# 	byte packetLength;
# 	byte 0x0E;
# 	byte numberOfPlayers;		// How many players are joining?
# 	struct playerArray[numberOfPlayers] {
# 		byte playerID;
# 		byte teamAndChar;	// See below
# 		byte furColor[4];
# 		char playerName;	// playerName is null terminated
#       [0x20, 0x28, 0x11, 0x01, 0x0A, 0x0D, 0x00, 0x00] // no clue, probably a plus thing
# 	}
# }

playerArray = [0x1, 0x1, 0x1, 0x1, 0x18] + [0x20, 0x28, 0x11, 0x01, 0x0A, 0x0D, 0x00, 0x00] + [ord(i) for i in "||Spaz ||Electro"] + [0x0]
_packet = [0x0E, 0x1] + playerArray
packet = bytes(bytearray([len(_packet)+1] + _packet))

print("""# struct joinDetails {
 	byte packetLength;
 	byte 0x0E;
 	byte numberOfPlayers;		// How many players are joining?
 	struct playerArray[numberOfPlayers] {
 		byte playerID;
 		byte teamAndChar;	// See below
 		byte furColor[4];
 		char playerName;	// playerName is null terminated
 	}
 }""")
hexdump.hexdump(packet)

tcp_client.sendall(packet)

time.sleep(5)

# # spectate packet
# tcp_client.send(b"\x03B!")
# # tcp_client.send(ChatMessage(1, "/spectate on"))
# tcp_client.send(ChatMessage(1, "/login botattack"))

# time.sleep(1)

# import threading

# END_CLIENT = False
# last_pong = 0x01

# posPacket = bytearray([0x3D, 0xA7, 0x1, 0x0, 0x1, 0xB5, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x1E, 0x0, 0x0, 0x0])

# def udp_recv():
#     global END_CLIENT
#     while not END_CLIENT:
#         global last_pong
#         msg, addr = udp_client.recvfrom(1024)
#         if addr != server_address:
#             continue

#         # first 2 are checksum
#         if msg[2] == 0x04:
#             last_pong = msg[3]

#         print("UDP")
#         hexdump.hexdump(msg)
#         print("UDP_PACKET_END")

#         pingPacket = bytes(bytearray([0x03, last_pong, 0x00, 0x00, 0x00, 0x00, 0x32, 0x34, 0x20, 0x20]))
#         udp_client.sendto(b"\xA7\xD2" + pingPacket, server_address)

# def ping_server():
#     global END_CLIENT
#     while not END_CLIENT:
#         posPacket[-3] += 1
#         if posPacket[-3] == 5:
#             posPacket[-3] = 0

#         if posPacket[5] == 0xB5:
#             posPacket[5] = 0xB6
#         else:
#             posPacket[5] = 0xB5
        
#         udp_send(posPacket)
#         time.sleep(1) 

# def udp_checksum(buffer):
#     x = 1
#     y = 1
#     for i in range(2, len(buffer)):
#         x += buffer[i]
#         y += x
#     buffer[0] = x % 251
#     buffer[1] = y % 251

# def udp_send(data):
#     udp_checksum(data)
#     udp_client.sendto(data, server_address)

# udp_thread = threading.Thread(target=udp_recv)
# udp_thread.start()

# ping_thread = threading.Thread(target=ping_server)
# ping_thread.start()

# try:
#     while True:
#         msg = tcp_client.recv(1024)
#         print("TCP")
#         hexdump.hexdump(msg)
#         print("TCP_PACKET_END")
#         # pingPacket = bytes(bytearray([0x03, last_pong, 0x00, 0x00, 0x00, 0x00, 0x32, 0x34, 0x20, 0x20]))
#         # udp_client.sendto(b"\xA7\xD2" + pingPacket, server_address)
#         # tcp_client.send(msg)

#         if len(msg) >= 2:
#             # 05 49 01 10 27
#             # if msg[0] == 0x05 and msg[1] == 0x49:
#             if msg[0] == 0x00 and msg[1] == 0x07:
#                 # disconnect
                
#                 disconnectMessage = msg[4]
#                 print(f"Disconnected for {parse_disconnect_message(disconnectMessage)}")
                
#                 raise KeyboardInterrupt()
#             if msg[0] == 0x27 and msg[1] == 0x16:
#                 send_packet_array("login")

#         if msg == b"":
#             raise KeyboardInterrupt()
# except KeyboardInterrupt:
#     END_CLIENT = True
#     observer.stop()

# observer.join()
# print("Waiting for UDP thread to stop...")
# udp_thread.join()
