import socket
import threading
import uuid
import os
import hexdump

# Configuration
proxy_host = "127.0.0.1"
proxy_port = 1337

jj2_host = "127.0.0.1"
jj2_port = 10052

proxyfileindex = 0

# BUFFER_SIZE = 16384
BUFFER_SIZE = 32768
USE_COMBINED_FOLDER = False
ASK_INPUT_TCP = False
ASK_INPUT_UDP = False
LOG_TCP_PACKETS = False
LOG_UDP_PACKETS = False

END_PROXY = False

sessionuid = uuid.uuid4()
os.mkdir(f"tcp/{sessionuid}")
os.mkdir(f"udp/{sessionuid}")

def udp_proxy():
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_sock.bind((proxy_host, proxy_port))
    jj2_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while True:
        global proxyfileindex, END_PROXY, LOG_UDP_PACKETS
        data: bytes = b""

        try:
            data, client_addr = udp_sock.recvfrom(BUFFER_SIZE)
        except ConnectionResetError:
            END_PROXY = True
        if END_PROXY: break

        if data[2] == 0x03 or data[2] == 0x04:
            print(data)
        
        def forward_udp_to_jj2():
            jj2_sock.sendto(data, (jj2_host, jj2_port))
            response, _ = jj2_sock.recvfrom(BUFFER_SIZE)
            if response[2] == 0x03 or response[2] == 0x04:
                print(data)
            udp_sock.sendto(response, client_addr)

        if ASK_INPUT_UDP:
            if input(b"UDP " + bytearray(data)) == "y":
                open(f"udp/{sessionuid}/{proxyfileindex}" if not USE_COMBINED_FOLDER else f"combined/udp{proxyfileindex}", "wb").write(data)
        else:
            if LOG_UDP_PACKETS:
                print(f"UDP {hexdump.hexdump(data, result='return')}")
            if data[2] == 0x01:
                open(f"animatepacketsamples/{proxyfileindex}", "wb").write(data)
                proxyfileindex += 1
        proxyfileindex += 1

        udp_thread = threading.Thread(target=forward_udp_to_jj2)
        udp_thread.start()


def tcp_proxy():
    tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_sock.bind((proxy_host, proxy_port))
    tcp_sock.listen(5)

    while True:
        client_sock, client_addr = tcp_sock.accept()
        jj2_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        if END_PROXY: break

        def forward_tcp_to_jj2():
            jj2_sock.connect((jj2_host, jj2_port))

            def forward_data(sock1, sock2):                
                while True:
                    global proxyfileindex, LOG_TCP_PACKETS, LOG_UDP_PACKETS
                    data: bytes = sock1.recv(BUFFER_SIZE)
                    if END_PROXY: break

                    if sock1 is client_sock and ASK_INPUT_TCP:
                        if input(b"TCP " + bytearray(data)) == "y":
                            open(f"tcp/{sessionuid}/{proxyfileindex}" if not USE_COMBINED_FOLDER else f"combined/tcp{proxyfileindex}", "wb").write(data)
                    else:
                        if LOG_TCP_PACKETS and sock1 is client_sock:
                            print(f"TCP {hexdump.hexdump(data, result='return')}")
                    if sock1 is jj2_sock:
                         if data[1] == 0x49:
                             print("Spotted newping packet!")
                             LOG_TCP_PACKETS = True
                             LOG_UDP_PACKETS = True
                    proxyfileindex += 1
                    
                    if not data:
                        break
                    sock2.sendall(data)

            thread1 = threading.Thread(target=forward_data, args=(client_sock, jj2_sock))
            thread2 = threading.Thread(target=forward_data, args=(jj2_sock, client_sock))

            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()

        tcp_thread = threading.Thread(target=forward_tcp_to_jj2)
        tcp_thread.start()

if __name__ == "__main__":
    udp_thread = threading.Thread(target=udp_proxy)
    tcp_thread = threading.Thread(target=tcp_proxy)

    print(f"Starting UDP, TCP threads, Proxy port: {proxy_port}")

    try:
        udp_thread.start()
        tcp_thread.start()
    except KeyboardInterrupt:
        END_PROXY = True
