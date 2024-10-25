import clang.cindex

def get_function_names(node):
    if node.kind == clang.cindex.CursorKind.from_id("FUNCTION_DECL"):
        yield node.spelling
    for child in node.get_children():
        yield from get_function_names(child)

clang.cindex.Config.set_library_file("G:\\steve\\external-libclang-master\\bin\\x64\\libclang.dll")

index = clang.cindex.Index.create()
translation_unit = index.parse("main.cpp", args=['-std=c++11'])

for function_name in get_function_names(translation_unit.cursor):
    print(f"Found function: {function_name}")
