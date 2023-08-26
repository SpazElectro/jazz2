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
        
        print(event.src_path)
        tcp_client.send(ChatMessage(1, "/r"))

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

set_global_sockets(tcp_client, udp_client)
send_packet_array("login")

time.sleep(1)

tcp_client.send(ChatMessage(1, "/spectate on"))
tcp_client.send(ChatMessage(1, "/login botattack"))

time.sleep(1)

try:
    while True:
        msg = tcp_client.recv(1024)
        # if addr != server_address:
        #     continue
        print("PACKET_END" if not hexdump.hexdump(msg) else "UNDEFINED")
        if len(msg) >= 2:
            if msg[0] == 0x00 and msg[1] == 0x07:
                # disconnect
                
                disconnectMessage = msg[4]
                print(f"Disconnected for {parse_disconnect_message(disconnectMessage)}")
                
                raise KeyboardInterrupt()
        if msg == b"":
            raise KeyboardInterrupt()
        # pingPacket = bytes(bytearray([0x03, 0x1, 0x00, 0x00, 0x00, 0x00, 0x32, 0x34, 0x20, 0x20]))
        # udp_client.sendto(checksum(pingPacket) + pingPacket, server_address)
except KeyboardInterrupt:
    observer.stop()

observer.join()
