```c
// packet id = 0x05
struct client {
   char numberInList;
};

// packet id = 0x06
struct server {
   char numberInList;
   char ??;
   char ??;
   char ??[2];
   char version[4];
   char ??;
   char playerCount;
   char gameMode;
   char playerLimit;
   jj2_string serverName;
   char ??[2];
}
```

Example packet:
```
Client sends to server:
06 0D ; checksum
05 ; pkt id
00 ; number in list
-------------
Server sends to client:
99 F3 ; checksum
06 ; pkt id
00 ; number in list
F3 ; ?
00 ; ?
00 00 ; ?
32 34 20 20 ; version
01 ; player count
01 ; ?
02 ; game mode
20 ; max players
04 74 65 73 74 00 ; server name
02 00 ; ?
```

