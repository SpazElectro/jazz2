import zlib
import struct

def compress_file(input_filename, output_filename):
    with open(input_filename, 'rb') as input_file:
        data = input_file.read()

    uncompressed_size = len(data)  # Get the uncompressed size of the data
    compressed_data = zlib.compress(data)

    with open(output_filename, 'wb') as output_file:
        # Write the uncompressed size as a 4-byte integer
        output_file.write(struct.pack('I', uncompressed_size))
        # Write the compressed data
        output_file.write(compressed_data)

if __name__ == "__main__":
    input_filename = 'output.asvidu'
    output_filename = 'final.asdat'
    compress_file(input_filename, output_filename)
    print(f"File compressed and saved as {output_filename}")