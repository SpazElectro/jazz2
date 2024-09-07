import json

f = open("classes_unstripped.json").read()
f = json.loads(f)
output = {}

{
    "name": "onChat",
    "description": 'onChat is called whenever a chat message pops up in game. onLocalChat is called only when chat is received from players on the same machine the script is executing on. A return value of true indicates that the chat message should be suppressed, whereas a return value of false will cause the message to be handled normally. clientID is a unique ID of the game client that sent the chat message. stringReceived is the text of the chat message that was received. chatType can take one of the following values: NORMAL, TEAMCHAT, WHISPER amd ME.\nAny message beginning with "/" is interpreted as a command, not as chat, and so will not be passed to either of these hooks. Messages beginning with "!" will, though, as will arguments of commands /whisper (and its aliases, /w and @) and /me.\n',
    "full": "void onChat(int clientID, string &in stringReceived, CHAT::Type chatType)",
    "type": "event",
    "arguments": [
        {"type": "int", "name": "clientID", "attributes": [], "items": []},
        {
            "type": "string",
            "name": "stringReceived",
            "attributes": [{"type": "&in"}],
            "items": [],
        },
        {
            "type": "CHAT::Type",
            "name": "chatType",
            "attributes": [],
            "items": ["NORMAL", "TEAMCHAT", "WHISPER", "ME"],
        },
    ],
    "attributes": [{"type": "updated"}, {"type": "deprecated"}],
}


for x in f.keys():
    v = f[x]
    output[x] = []
    
    for y in v:
        t = y
        for k in t["arguments"]:
            if k.get("items"):
                del k["items"]
        output[x].append(t)

open("classes.json", "w").write(json.dumps(output))