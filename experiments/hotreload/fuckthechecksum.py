def udpchecksum(buffer):
    size = len(buffer)
    
    # Let x and y be the first 2 bytes respectively and initialize them with value of 1
    x = 1
    y = 1
    
    # The main formula
    for i in range(size):
        x += buffer[i]
        y += x
    
    # Finalizing
    new_buffer = bytearray(buffer)
    new_buffer[0] = x & 0xFF
    new_buffer[1] = y & 0xFF
    return new_buffer

# Example usage
input_data = bytearray([0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80])
calculated_checksum = udpchecksum(input_data)
print("Calculated Checksum:", calculated_checksum)