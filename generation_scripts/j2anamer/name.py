import os

namesInOrder = open("list").read().split("\n")
nameId = 0

for x in os.listdir("../iconbuilder/j2a/Anims-j2a"):
    os.rename(f"../iconbuilder/j2a/Anims-j2a/{x}/", f"../iconbuilder/j2a/Anims-j2a/{namesInOrder[nameId]}/")
    
    print(f"Renamed {x} to {namesInOrder[nameId]}")
    
    nameId += 1
