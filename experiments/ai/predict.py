from base import SimpleChatbot

model_path = "chatbot_model.pkl"

chatbot = SimpleChatbot()
chatbot.load_model(model_path)

import discord

class MyClient(discord.Client):
    async def on_ready(self):
        print(f"Logged on as {self.user}!")
    
    async def on_message(self, message: discord.Message):
        if message.channel.name == "bot-chanel":
            if message.content.startswith("!c "):
                prediction = chatbot.predict(message.content[3:])

                if bytearray(prediction, "utf8") == bytearray(b''):
                    await message.channel.send("(No response)")
                else:
                    await message.channel.send(prediction)



intents = discord.Intents.default()
intents.message_content = True

client = MyClient(intents=intents)
client.run(open("token.txt").read())
