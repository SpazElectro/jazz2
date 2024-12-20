import struct, zlib
from typing import List
from prodict import Prodict

data = []

class ScannedTilesetResult(Prodict):
    Signature: bytes
    Title: bytes
    Version: bytes
    FileSize: bytes
    CRC32: bytes
    Errors: List[str]
    Warnings: List[str]

def pop(size) -> bytes:
    global data
    result = data[:size]
    data = data[size:]
    return bytes(result)

def printall(*kwargs):
    for k in kwargs:
        print(k)

def printtable(tbl):
    if type(tbl) == type({}):
        # dict
        for i, k in enumerate(tbl):
            print(f"{i} - {k}: {tbl[k]}")
    elif type(tbl) == type([]):
        # array
        for i, k in enumerate(tbl):
            print(f"{i}: {k}")
    else: print(tbl)

def scanTileset(_data) -> ScannedTilesetResult:
    errs = []
    warns = []

    global data
    data = _data

    # LEVL header
    Copyright = pop(180)
    Magic = pop(4)
    
    if Magic != b"TILE":
        errs.append("fake-j2t")
        return ScannedTilesetResult.from_dict({"Errors": errs, "Warnings": []})
    
    signature = pop(4).decode("utf-8", "ignore")
    title = pop(32).decode("utf-8", "ignore")
    version = pop(2).decode("utf-8", "ignore")
    filesize = pop(4).decode("utf-8", "ignore")
    crc32 = pop(4).decode("utf-8", "ignore")
    # we don't need the rest for now

    return ScannedTilesetResult.from_dict({
        "Signature": signature,
        "Title": title,
        "Version": version,
        "FileSize": filesize,
        "CRC32": crc32,
        "Errors": errs,
        "Warnings": warns
    })