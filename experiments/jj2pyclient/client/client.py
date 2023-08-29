import socket
import threading
import struct

from typing import List, Tuple

import multitimer

posPacket = bytearray([0x3D, 0xA7, 0x1, 0x0, 0x1, 0xB5, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x1E, 0x0, 0x0, 0x0])

class JJ2Client:
    _joiningData1 = bytearray([0x9, 0xF, 0x0, 0x0, 0x32, 0x34, 0x20, 0x20, 0x1])

    def __init__(self, name):
        if len(self.name) > 20:
            raise Exception("Name is too long, name cannot be over 19 characters")
        
        self.name = name
        
        self.tcpSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.udpSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.serverAddr = ("127.0.0.1", 10052)
        self.connected = False
        self.jj2_version = None

        self.animTimer = None
    
    def join_server(self, addr: Tuple[str, int]):
        if isinstance(addr[0], str) == False or isinstance(addr[1], int) == False:
            raise Exception("Address should be in a tuple format, ex: (\"127.0.0.1\", 10052)")
        
        self.serverAddr = addr
        
        self.animTimer = multitimer.MultiTimer(interval=0.125, function=self.anim_timer_tick)
        if self.tcpSock.connect_ex(addr) == 0:
            self.tcpSock_connected()
    
    def tcpSock_connected(self):
        self.connected = True
        local_port_bytes = struct.pack("!H", self.serverAddr[1])

        self._joiningData1[2:4] = local_port_bytes

        threading.Thread(target=self.tcpSock_startRecv).start()
        threading.Thread(target=self.udpSock_startRecv).start()

        if self.jj2_version != None:
            versionBytes = self.jj2_version.encode("ascii")
            if len(versionBytes) <= 4:
                self._joiningData1[4:4 + len(versionBytes)] = versionBytes


        self.udp_send(self._joiningData1)
        # Winsock2_DataArrival
        # line 220
    
    def tcpSock_startRecv(self):
        while True:
            data = self.tcpSock.recv(1024)

            # Dim dataLength As Integer = Winsock2.Client.EndReceiveFrom(ar, ar.AsyncState)
            # TotalBytesRecv += dataLength
            # ConnectionTimeOut = 30
            # Dim recv(dataLength - 1) As Byte
            # Array.Copy(BufferUDP, recv, dataLength)
            # If _plusServer = False Then
            #     Winsock2_DataArrival_Read(recv)
            # Else
            #     Winsock2_DataArrival_Read_Plus(recv)
            # End If
            
            print("TCPRecv")
        
    def udpSock_startRecv(self):
        while True:
            data, addr = self.udpSock.recvfrom(1024)
            if addr != self.serverAddr:
                continue

            print("UDPRecv")

    def udp_checksum(self, buffer):
        x = 1
        y = 1
        for i in range(2, len(buffer)):
            x += buffer[i]
            y += x
        buffer[0] = x % 251
        buffer[1] = y % 251

    def udp_send(self, data):
        self.udp_checksum(data)
        self.udpSock.sendto(data, self.serverAddr) # type: ignore

    def anim_timer_tick(self):
        if self.connected:
            posPacket[-3] += 1
            if posPacket[-3] == 5:
                posPacket[-3] = 0

            if posPacket[5] == 0xB5:
                posPacket[5] = 0xB6
            else:
                posPacket[5] = 0xB5
            
            self.udp_send(posPacket)
