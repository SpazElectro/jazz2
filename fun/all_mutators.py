import pydirectinput, os, time

pydirectinput.PAUSE = 0

print("5 second pause")
time.sleep(5)
print("enabled")

for x in os.listdir("F:\\Games\\Jazz2\\cache\\"):
    if x.endswith(".mut"):
        pydirectinput.press("t")
        time.sleep(0.1)
        pydirectinput.write(f"/mutators {x} on")
        pydirectinput.press("enter")
        time.sleep(0.1)
