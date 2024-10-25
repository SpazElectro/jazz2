import sys
import socket
import os
import importlib
import threading
from shared import *

shutdown_event = threading.Event()
tcp_server = None
udp_server = None

def import_packet(proto, name, data, addr, tcp_socket, udp_socket):
    module = importlib.import_module(f"pkts.{proto}.{name}")
    module.pkt(data, addr, tcp_socket, udp_socket)
    del sys.modules[f"pkts.{proto}.{name}"]

def handle_udp():
    global udp_server
    udp_server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_server.bind(("0.0.0.0", 10052))
    
    try:
        while not shutdown_event.is_set():
            udp_server.settimeout(1.0)  # Timeout to allow shutdown check
            try:
                data, addr = udp_server.recvfrom(2048)
                checksum = data[0:2]
                expected_checksum = calculate_checksum(bytearray(data[2:]))
                if bytearray(checksum) != expected_checksum:
                    print("Checksums do not match!")
                    print(f"Got {checksum} VS Expected {expected_checksum}")
                    continue

                packet_id = data[2]
                if os.path.exists(f"./pkts/udp/{str(packet_id)}.py"):
                    import_packet("udp", str(packet_id), data, addr, None, udp_server)
                else:
                    print(f"UDP Unknown packet id! {packet_id}")
            except socket.timeout:
                continue
    except KeyboardInterrupt:
        print("UDP server shutting down...")
    finally:
        udp_server.close()

def handle_tcp():
    global tcp_server
    tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_server.bind(("0.0.0.0", 10052))
    tcp_server.listen(5)
    tcp_server.setblocking(False)  # Set non-blocking mode
    
    def handle_client(client_socket):
        try:
            while not shutdown_event.is_set():
                client_socket.settimeout(1.0)  # Timeout to allow shutdown check
                try:
                    header = client_socket.recv(2)
                    if not header:
                        break
                    length = header[0]
                    packet_id = header[1]

                    data = client_socket.recv(length - 2)
                    if len(data) + 2 != length:
                        print(f"TCP packet length mismatch: Expected {length}, got {len(data) + 2}")
                        continue

                    if os.path.exists(f"./pkts/tcp/{str(packet_id)}.py"):
                        import_packet("tcp", str(packet_id), header + data, None, client_socket, udp_server)
                    else:
                        print(f"TCP Unknown packet id! {packet_id}")
                except socket.timeout:
                    continue
        finally:
            client_socket.close()

    try:
        while not shutdown_event.is_set():
            try:
                client_socket, addr = tcp_server.accept()
                print(f"Accepted TCP connection from {addr}")
                client_thread = threading.Thread(target=handle_client, args=(client_socket,))
                client_thread.start()
            except BlockingIOError:
                # No connections, continue to check shutdown_event
                pass
            except socket.timeout:
                continue
    except KeyboardInterrupt:
        print("TCP server shutting down...")
    finally:
        tcp_server.close()


import signal

def signal_handler(sig, frame):
    print("Interrupt received, shutting down...")
    shutdown_event.set()

def main():
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    udp_thread = threading.Thread(target=handle_udp)
    tcp_thread = threading.Thread(target=handle_tcp)
    
    udp_thread.start()
    tcp_thread.start()
    
    try:
        udp_thread.join()
        tcp_thread.join()
    except KeyboardInterrupt:
        print("Main thread interrupted. Signaling shutdown...")
        shutdown_event.set()
        udp_thread.join()
        tcp_thread.join()

if __name__ == "__main__":
    main()