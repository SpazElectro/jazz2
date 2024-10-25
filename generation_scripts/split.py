def split_file(input_file, num_parts):
    # Open the input file in binary mode
    with open(input_file, 'rb') as f:
        # Read the entire file and store it in memory
        file_data = f.read()

    # Calculate the size of each part (rounded up)
    part_size = -(-len(file_data) // num_parts)

    # Iterate over the number of parts
    for i in range(num_parts):
        # Calculate the start and end indices of the current part
        start = i * part_size
        end = min((i + 1) * part_size, len(file_data))

        # Create the output file for the current part
        output_file = f'{input_file}.part{i}'

        # Write the current part to the output file
        with open(output_file, 'wb') as f:
            f.write(file_data[start:end])

        # Print a message to indicate progress
        print(f'Wrote {end - start} bytes to {output_file}')

    print('File splitting complete.')

# Example usage:
split_file('apifunctions.json', 10)
