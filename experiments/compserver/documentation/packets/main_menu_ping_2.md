```c
// packet id = 0x03
struct client {
    char ??[5];
    char ??[3];
    char version[2];
};

// packet id = 0x04
struct server {
    char ??[5];
    char ??;
};
```

UDP Sent unknown_03
3  0.0.0.0:51161  :0  12  SendTo  
03 00 F8 3F 00 00 32 34 20 20                .....?..24  

UDP Got unknown_04
4  :0  0.0.0.0:51161  9  RecvFrom  
04 00 F8 3F 00 00 80                         .....?...
