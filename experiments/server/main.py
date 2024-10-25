import socket
import threading, time

# UDP Server

class UDPServerThread(threading.Thread):
    def __init__(self, host, port):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port
        
    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.bind((self.host, self.port))
            while True:
                data, addr = sock.recvfrom(1024)

                with open(f"packets/udp/{time.time()}", "wb") as f:
                        f.write(data)
                
# TCP Server

class TCPServerThread(threading.Thread):
    def __init__(self, host, port):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port
        
    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.bind((self.host, self.port))
            sock.listen(1)
            conn, addr = sock.accept()
            with conn:
                print(f"Connected to {addr}")

                while True:
                    data = conn.recv(1024)
                    if not data:
                        break

                    with open(f"packets/tcp/{time.time()}", "wb") as f:
                        f.write(data)
                    
# Start servers

udp_server = UDPServerThread('localhost', 10052)
tcp_server = TCPServerThread('localhost', 10054)

udp_server.start()
tcp_server.start()

udp_server.join()
tcp_server.join()
