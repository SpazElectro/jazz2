import pydirectinput, time

pydirectinput.PAUSE = 0

print("5 second pause")
time.sleep(5)
print("enabled")

delay = 0.08
enterdelay = 0.1
chatdelay = 0.1

while True:
    pydirectinput.press("t")
    time.sleep(chatdelay)
    pydirectinput.write("whopper")
    time.sleep(delay)
    pydirectinput.press("enter")
    time.sleep(enterdelay)
