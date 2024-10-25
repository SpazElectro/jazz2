trace = open("trace.txt").read()

inside_packet = False
packet_type = ""

tcp = ["Send", "Recv"]
udp = ["SendTo", "RecvFrom"]

tcp_packets = {
    "0F": "join_request",
    "10": "join_details",
}
udp_packets = {
    "01": "animation",
    "03": "main_menu_ping_2",
    "04": "main_menu_ping_2",
    "05": "main_menu_ping",
    "06": "main_menu_ping",
}

who_am_i = "client"
output = f"# This log is registered as being the {who_am_i}\n"
index = 0
captured = False

for line in trace.splitlines():
    if line.strip() == "":
        inside_packet = False
        continue
    if line.strip().split(" ")[0].isdigit() and not inside_packet:
        index += 1
        end = line.strip().split(" ")[-1]
        
        output += f"{index:02} "
        if end in tcp:
            output += "TCP "
            packet_type = "TCP"
        elif end in udp:
            output += "UDP "
            packet_type = "UDP"
        else:
            print(f"Unknown packet type! {end} :::: {line.strip()}")
            break
        if end.startswith("Recv"):
            #output += "S->C "
            output += "Sent" if who_am_i == "server" else "Got"
        else:
            #output += "C->S "
            output += "Got" if who_am_i == "server" else "Sent"
        output += " "
        inside_packet = True
        captured = False
    elif inside_packet and line.split(" ")[0].isdigit() and not captured:
        content = " ".join(line.split(" ")[2:]).strip()
        packet_id = "UNK"
        if packet_type == "TCP":
            packet_id = content.split(" ")[1]
        else:
            packet_id = content.split(" ")[2]
        output += f"{(tcp_packets if packet_type == 'TCP' else udp_packets).get(packet_id, f'unknown_{packet_id}')}\n"
        captured = True

open("output.txt", "w").write(output)
