from base import SimpleChatbot

import json

data = json.load(open("data.json", encoding="utf8"))["messages"]

fixedData = []

for x in data:
    fixedData.append(x["content"])

def generate_train():
    inputs = []
    outputs = []
    index = 0

    for message in fixedData:
        if index >= 5000:
            break
        if index % 2 == 0:
            index += 1
            outputs.append(message)
            continue
        
        inputs.append(message)
        
        index += 1
    
    # open("io.i", "w").write(json.dumps(inputs))
    # open("io.o", "w").write(json.dumps(outputs))

    print("Finished getting train info")
    
    return inputs, outputs

train = generate_train()

chatbot = SimpleChatbot()
chatbot.train(train[0], train[1], "chatbot_model.pkl")
