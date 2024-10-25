```c
// shared
struct TCP_Packet {
	byte packetLength;	// Length/size of the packet
	byte packetID;		// Identified type of packet
	misc extraData;		// Can be of variable size
};

// S2C
struct disconnectClient {
	byte 0x08;		// packetLength, always 8 here
	byte 0x0D;		// packetID
	byte disconnectMessage;	// See table below
	byte socketID;		// The ID of the disconnected socket
	char serverVersion[4]; // for 1.24 it's "24  " (two spaces, not special characters) ((0x32 0x31 0x20 0x20) for version)
}

// C2S
struct joinDetails {
	byte packetLength;
	byte 0x0E;
	byte numberOfPlayers;		// How many players are joining?
	struct playerArray[numberOfPlayers] {
		byte playerID;
		byte teamAndChar;	// See below
		byte furColor[4];
		char playerName;	// playerName is null terminated
	}
}

// S2C
struct joinNotification {
	byte packetLength;
	byte 0x11;
	byte socketID;		// The joiner(s) have this socket ID
	byte numberOfPlayers;
	struct playerArray[numberOfPlayers] {
		byte playerID;
		byte teamAndChar;	// See Joining details
		byte furColor[4];
		char playerName;	// playerName is null terminated
	}
}
```


| disconnectMessage |   Description  |
|-------------------|:--------------:|
| 0×01 |	Server full              |
| 0×02 |	Version different        |
| 0×03 |	?                        |
| 0×04 |	Error during handshaking |
| 0×05 |	Feature not supported in shareware                            |
| 0×06 |	Error downloading level  |
| 0×07 |	Connection lost          |
| 0×08 |	Winsock error            |
| 0×09 |	Connection timed out     |
| 0×0A |	Server stopped           |
| 0×0B |	Kicked off               |
| 0×0C |	Banned                   |
| else |	Unknown error            |
