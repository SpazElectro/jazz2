import os
import datetime

first_file = "predefined/as.predefined"
output_file = "../include/as.predefined"

other_files = ["predefined/jj2_additional.predefined", "predefined/jj2.predefined"]

with open(first_file, "r") as f:
    lines = f.readlines()

for other_file in other_files:
    with open(other_file, "r") as f:
        other_lines = f.readlines()
    lines += other_lines
    # remove all lines that end with "// REMOVE_LINE"
    lines = [line for line in lines if not line.strip().endswith("// REMOVE_LINE")]

with open(output_file, "w") as f:
    f.write(f"// Generated on {datetime.datetime.now()}\n")
    f.writelines(lines)

print(f"Generated {len(lines)} lines!")
