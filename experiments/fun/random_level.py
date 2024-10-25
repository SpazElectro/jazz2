import pydirectinput, os, time, random

pydirectinput.PAUSE = 0

print("5 second pause")
time.sleep(5)
print("enabled")

levels = []

for x in os.listdir("F:\\Games\\Jazz2\\cache\\"):
    if x.endswith(".j2l"):
        levels.append(x)

level = levels[random.randint(0, levels.__len__())]

print(level)

pydirectinput.write(f"t")
time.sleep(1)
pydirectinput.write(f"/c {level}")
pydirectinput.press("enter")