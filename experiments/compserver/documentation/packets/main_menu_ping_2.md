```c
// packet id = 0x03
struct client {
    char numberInList;
    char ping[4];
    char version[4];
};

// packet id = 0x04
struct server {
    char numberInList;
    char ping[4]; // this controls the ping, the closer it is to the original ping value, the "better" the ping
    // gamemode, password, etc
    char flags;
};
```

UDP Sent unknown_03
3  0.0.0.0:51161  :0  12  SendTo  
03 00 F8 3F 00 00 32 34 20 20                .....?..24  

UDP Got unknown_04
4  :0  0.0.0.0:51161  9  RecvFrom  
04 00 F8 3F 00 00 80                         .....?...
