import pydirectinput, os, time

pydirectinput.PAUSE = 0

print("5 second pause")
time.sleep(5)
print("enabled")

for x in os.listdir("F:\\Games\\Jazz2\\cache\\"):
    if x.endswith(".mut"):
        pydirectinput.write("t")
        time.sleep(2)
        pydirectinput.write(f"/mutators {x} on")
        time.sleep(1)
        pydirectinput.press("enter")
        time.sleep(2)
