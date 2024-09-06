def calculate_checksum(data: bytearray):
    x, y = 1, 1
    for p in data:
        x += p
        y += x
    return bytearray([x%251, y%251])

# print(calculate_checksum(bytearray(b"\x05\x00")).hex(" ") == "06 0d")

# data: [0x1, bytearray(b"162")]
def create_udp_packet(data):
    proper_data = bytearray([0, 0])
    for x in data:
        if type(x) == bytearray:
            for y in x:
                proper_data.append(y)
        else:
            proper_data.append(x)
    
    checksum = calculate_checksum(proper_data[2:])
    proper_data[0] = checksum[0]
    proper_data[1] = checksum[1]

    return proper_data

def create_tcp_packet(data):
    # struct TCP_Packet {
    #     byte packetLength;	// Length/size of the packet
    #     byte packetID;		// Identified type of packet
    #     misc extraData;		// Can be of variable size
    # };
    proper_data = bytearray([0])
    for x in data:
        if type(x) == bytearray:
            for y in x:
                proper_data.append(y)
        else:
            proper_data.append(x)
    proper_data[0] = len(proper_data)
    return proper_data

def create_jj2_string(text: str):
    data = bytearray([len(text)])
    for x in text:
        data.append(ord(x))
    data.append(0)
    return data
