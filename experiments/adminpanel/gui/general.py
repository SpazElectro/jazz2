import websockets.sync.client as wsc
import flet as ft

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
    
connection: wsc.ClientConnection
isConnected = False

def getIsConnected():
    return isConnected

def websocket_loop(msgcallback, playercallback, page: ft.Page):
    global connection, isConnected
    isConnected = False
    connection = wsc.connect("ws://localhost:1337")
    isConnected = True

    if page.route == "/":
        if page.controls != None:
            print("p")
            for control in page.controls: # type: ignore
                print("a control")
                if isinstance(control, ft.Text):
                    print(f"constrained {control.key}")
                    if control.key == "isConnected":
                        print("isConnected!")
                        control: ft.Text = control
                        control.value = "Connected: true"
                        print("found")
                        
            page.update()
    print("Connected!")

    while True:
        data = connection.recv()
        ctype, content = disassemble_packet(data) # type: ignore
        
        print(f"Packet type: {ctype}")
        print(f"Packet content: {content}")

        if ctype == "message":
            msgcallback(content)
        elif ctype == "players":
            playercallback(content)

def websocket_send(msg):
    global connection
    connection.send(msg)