import json
import os

def remove_documentation(data):
    if isinstance(data, dict):
        # data.pop("description", None)
        data.pop("arguments", None)
        data.pop("attributes", None)

        for key in list(data.keys()):
            remove_documentation(data[key])
    elif isinstance(data, list):
        for item in data:
            remove_documentation(item)

def process_json(input_file):
    with open(input_file, 'r') as file:
        data = json.load(file)

    remove_documentation(data)
    json_string = json.dumps(data)

    chunk_size = 2000000 #12000
    chunks = [json_string[i:i + chunk_size] for i in range(0, len(json_string), chunk_size)]

    for index, chunk in enumerate(chunks):
        output_file = f'cleaned_chunk_{index + 1}.json'
        with open(output_file, 'w') as file:
            file.write(chunk)
        print(f"Chunk {index + 1} saved to {output_file}.")

input_file = 'bad.json'
process_json(input_file)

print("Documentation removed and JSON split into chunks.")
