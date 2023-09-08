import websockets.sync.client as wsc

def assemble_packet(type, content):
    return f"{type}:{content}"

def disassemble_packet(message):
    parts = message.split(":", 1)
    if len(parts) == 2:
        type = parts[0].strip()
        content = parts[1].strip()
        return [type, content]
    else:
        return None
    
connection = None
def websocket_loop(msgcallback, playercallback):
    global connection
    connection = wsc.connect("ws://localhost:1337")
    print("Connected!")

    while True:
        data = connection.recv()
        ctype, content = disassemble_packet(data)
        
        print(f"Packet type: {ctype}")
        print(f"Packet content: {content}")

        if ctype == "message":
            msgcallback(content)
        elif ctype == "players":
            playercallback(content)

def websocket_send(msg):
    global connection
    connection.send(msg)