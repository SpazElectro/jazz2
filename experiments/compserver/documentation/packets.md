# WPE
Send, Recv       => TCP
SendTo, RecvFrom => UDP

# General

```c
// char  => 1 byte
// short => 2 bytes

// example: "test" would become [len 04] [text 74 65 73 74] [c_string 00]
struct jj2_string {
   char length;
   char serverName[length];
};

struct tcp_packet {
   char length; // including the length and packetId bytes
   char packetId;
};

struct udp_packet {
   short checksum;
   char packetId;
};
```

Player join packet flow:
03 TCP 0F C->S
04 TCP 10 S->C
05 UDP 15 C->S
06 TCP 3F C->S
07 TCP 0E C->S
08 TCP 3F S->C
09 TCP 5B S->C
10 UDP 02 S->C
11 UDP 15 C->S
12 UDP A6 S->C
13 UDP 55 C->S
14 TCP 51 S->C
15 TCP 1A C->S
16 TCP 44 S->C
17 TCP 4C S->C
-- STOP HERE
-- player state is handled with packet id 01

# Packets

## UDP
[Main Menu ping](packets/main_menu_ping.md)

## TCP
