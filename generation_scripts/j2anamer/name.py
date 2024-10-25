import os

namesInOrder = open("list").read().split("\n")
nameId = 0

for x in os.listdir("../j2a/Anims-j2a"):
    os.rename(f"../j2a/Anims-j2a/{x}/", f"../j2a/Anims-j2a/{namesInOrder[nameId]}/")
    
    print(f"Renamed {x} to {namesInOrder[nameId]}")
    
    nameId += 1
