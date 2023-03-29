def hex_string_to_bytes(hex_string):
    hex_values = hex_string.split()
    byte_values = [int(hex_value, 16) for hex_value in hex_values]
    return bytes(byte_values)

def pad_string_to_bytes(string, length):
    truncated_string = string[:length]
    padded_string = truncated_string.ljust(length, b'\0')

    return bytes(padded_string)


copyrightBytesStr = "20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 4A 61 7A 7A 20 4A 61 63 6B 72 61 62 62 69 74 20 32 20 44 61 74 61 20 46 69 6C 65 0D 0A 0D 0A 20 20 20 20 20 20 20 20 20 52 65 74 61 69 6C 20 64 69 73 74 72 69 62 75 74 69 6F 6E 20 6F 66 20 74 68 69 73 20 64 61 74 61 20 69 73 20 70 72 6F 68 69 62 69 74 65 64 20 77 69 74 68 6F 75 74 0D 0A 20 20 20 20 20 20 20 20 20 20 20 20 20 77 72 69 74 74 65 6E 20 70 65 72 6D 69 73 73 69 6F 6E 20 66 72 6F 6D 20 45 70 69 63 20 4D 65 67 61 47 61 6D 65 73 2C 20 49 6E 63 2E 0D 0A 0D 0A 1A"
restStr = "85 02 00 00 C8 27 0D 64 EB 00 00 00 6D 92 00 00 64 00 00 00 00 00 01 00 0E 00 00 00 10 00 00 00 22 00 00 00 20 20 00 00 78 DA ED D2 B1 4A C3 40 1C C0 E1 AB 5D 1C 05 67 21 BB E2 E0 E4 98 45 50 AC 93 08 8E C6 36 8B A4 AD D6 20 E8 53 14 67 9F C0 BE 88 0F E0 20 3E 8C F1 1F AC 83 2E 75 A8 2E 7E 1F FC 38 EE 2E DC 25 90 94 3E 74 F3 BC 1D 86 F7 29 9D 8C 46 C5 B0 1C 64 BD F2 A6 AC B2 F3 DB EC F8 B2 B8 CB F6 AA B2 5F 4F C6 F1 CC FE B8 8E F5 A3 83 D3 ED 8B 9D 3A FD BD 7E 71 5D 57 65 02 00 00 00 00 00 00 00 FE A9 EE D7 69 A7 ED 2C 16 F3 95 CF 59 4A B3 18 26 51 2F 5A 8D 16 ED BF 44 D3 28 9F B7 19 6D 44 EB F3 FD E7 B7 A6 D9 6D 9A E6 21 6A 2F 1D 44 4F D1 56 9C 35 ED 2C F1 E3 BE BD E8 EC 31 EE B9 4A E9 F5 F0 87 FB BF 6D CD FF 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 8B BD 03 00 69 2E 9A 78 DA ED D0 41 11 00 30 08 C0 30 8C C0 F0 AF 12 1E 73 41 72 57 03 8D 00 00 AE 7B 5B DB 00 27 E5 56 36 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 0D 42 52 00 7B 78 DA 63 60 80 00 79 28 04 00 02 7C 00 7D 78 DA ED C1 31 0D 00 00 0C 03 A0 D6 BF E9 4A D8 BD 04 48 00 00 00 00 00 00 00 80 EF 7A 1C 21 30 00 11"
copyrightBytes = hex_string_to_bytes(copyrightBytesStr)
rest = hex_string_to_bytes(restStr)

magicBytes = b"LEVL"
passwordHash = bytearray(b"\xBE\xBA\x00") # aka none

data = b""

data += copyrightBytes
data += magicBytes
data += passwordHash
data += b"\0" # hide level
data += pad_string_to_bytes(b"test level", 32)
data += b"\3" # version or something /shrug
data += b"\2" # file size
data += rest

with open("stvtestgenerate.j2l", "wb") as f:
    f.write(data)