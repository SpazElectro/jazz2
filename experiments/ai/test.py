from base import SimpleChatbot

chatbot = SimpleChatbot()
chatbot.load_model("chatbot_model.pkl")

input_text = input("User: ")

while input_text != "bye":
    output_text = chatbot.predict(input_text)
    
    if bytearray(output_text, "utf8") == bytearray(b''):
        print("Chatbot: (No response)")
    else:
        print("Chatbot: " + output_text)
    
    input_text = input("User: ")
