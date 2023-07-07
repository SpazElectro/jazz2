import struct, zlib
from typing import List
from prodict import Prodict

emptyPassword = bytearray(b'\xbe\xba\x00')

data = []

class ScannedLevelResult_Other(Prodict):
    fileSize: bytes
    crc32: bytes
    CData1: int
    UData1: int
    CData2: int
    UData2: int
    CData3: int
    UData3: int
    CData4: int
    UData4: int
    JCSHorizontalOffset: int
    Security1: bytes
    JCSVerticalOffset: int
    Security2: bytes
    SecAndLayer: bytes
    MinLight: int
    StartLight: int
    AnimCount: int
    VerticalSplitscreen: bool
    IsLevelMultiplayer: bool
    BufferSize: bytes
    _LevelName: str

class ScannedLevelResult(Prodict):
    LevelName: str
    PasswordProtected: bool
    HideLevel: bool
    Version: bytes
    other: ScannedLevelResult_Other
    TileSetName: str
    BonusLevel: str
    NextLevel: str
    SecretLevel: str
    MusicFile: str
    HelpStrings: List[str]
    Errors: list
    Warnings: list

def pop(size):
    global data
    result = data[:size]
    data = data[size:]
    return result

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

def scanLevel(_data) -> ScannedLevelResult:
    errs = []
    warns = []

    global data
    data = _data

    # LEVL header
    Copyright = pop(180)
    Magic = pop(4)
    
    if Magic != b"LEVL":
        errs.append("fake-j2l")
        return ScannedLevelResult.from_dict({"Errors": errs, "Warnings": []})
    
    PasswordHash = bytearray(pop(3))
    PasswordHash.reverse()
    PasswordProtected = not (PasswordHash == emptyPassword)

    HideLevel = pop(1) != b'\x00'
    LevelName = pop(32).split(b'\x00',1)[0]
    Version = pop(2)

    fileSize = pop(4)
    crc32    = pop(4)
    CData1   = int.from_bytes(pop(4))
    UData1   = int.from_bytes(pop(4))
    CData2   = int.from_bytes(pop(4))
    UData2   = int.from_bytes(pop(4))
    CData3   = int.from_bytes(pop(4))
    UData3   = int.from_bytes(pop(4))
    CData4   = int.from_bytes(pop(4))
    UData4   = int.from_bytes(pop(4))

    try:
        LevelName.decode()
    except UnicodeDecodeError:
        warns.append("weird-chars-in-title")
        LevelName = str(LevelName).encode()
    
    # Data1 (General Level Data)
    data1 = zlib.decompress(data[0:CData1])
    olddata = data
    data = data1 # to pop easily
    JCSHorizontalOffset = int.from_bytes(pop(2))
    Security1 = pop(2)
    JCSVerticalOffset = int.from_bytes(pop(2))
    Security2 = pop(2)
    SecAndLayer = pop(1)
    MinLight = int.from_bytes(pop(1))
    StartLight = int.from_bytes(pop(1))
    AnimCount = int.from_bytes(pop(2))
    VerticalSplitscreen = pop(1) != b'\x00'
    IsLevelMultiplayer = pop(1) != b'\x00'
    BufferSize = pop(4)
    _LevelName = pop(32).split(b'\x00',1)[0]
    TileSetName = pop(32).split(b'\x00',1)[0]
    BonusLevel = pop(32).split(b'\x00',1)[0]
    NextLevel = pop(32).split(b'\x00',1)[0]
    SecretLevel = pop(32).split(b'\x00',1)[0]
    MusicFile = pop(32).split(b'\x00',1)[0]
    HelpStrings = []

    try:
        _LevelName.decode()
    except UnicodeDecodeError:
        warns.append("weird-chars-in-_LevelName")
        _LevelName = str(_LevelName).encode()
    try:
        TileSetName.decode()
    except UnicodeDecodeError:
        warns.append("weird-chars-in-TileSetName")
        TileSetName = str(TileSetName).encode()
    try:
        BonusLevel.decode()
    except UnicodeDecodeError:
        warns.append("weird-chars-in-BonusLevel")
        BonusLevel = str(BonusLevel).encode()
    try:
        NextLevel.decode()
    except UnicodeDecodeError:
        warns.append("weird-chars-in-NextLevel")
        NextLevel = str(NextLevel).encode()
    try:
        SecretLevel.decode()
    except UnicodeDecodeError:
        warns.append("weird-chars-in-SecretLevel")
        SecretLevel = str(SecretLevel).encode()
    try:
        MusicFile.decode()
    except UnicodeDecodeError:
        warns.append("weird-chars-in-MusicFile")
        MusicFile = str(MusicFile).encode()

    for _ in range(16):
        helpStr = pop(512)
        helpStr = helpStr.split(b'\x00',1)[0]
        try:
            helpStr.decode()
        except UnicodeDecodeError:
            warns.append("weird-chars-in-helpstr")
            helpStr = str(helpStr).encode()
        HelpStrings.append(helpStr.decode())

    return ScannedLevelResult.from_dict({
        "LevelName": LevelName.decode(),
        "PasswordProtected": PasswordProtected,
        "HideLevel": HideLevel,
        "Version": Version.decode(),
        "other": {
            "fileSize": struct.unpack("<L", fileSize),
            "crc32": struct.unpack("<L", crc32),
            "CData1": CData1,
            "UData1": UData1,
            "CData2": CData2,
            "UData2": UData2,
            "CData3": CData3,
            "UData3": UData3,
            "CData4": CData4,
            "UData4": UData4,
            "JCSHorizontalOffset": JCSHorizontalOffset,
            "Security1": Security1.hex().__str__(),
            "JCSVerticalOffset": JCSVerticalOffset,
            "Security2": Security2.hex().__str__(),
            "SecAndLayer": SecAndLayer.hex().__str__(),
            "MinLight": MinLight,
            "StartLight": StartLight,
            "AnimCount": AnimCount,
            "VerticalSplitscreen": VerticalSplitscreen,
            "IsLevelMultiplayer": IsLevelMultiplayer,
            "BufferSize": BufferSize.hex().__str__(),
            "_LevelName": _LevelName
        },
        "TileSetName": TileSetName,
        "BonusLevel": BonusLevel,
        "NextLevel": NextLevel,
        "SecretLevel": SecretLevel,
        "MusicFile": MusicFile,
        "HelpStrings": HelpStrings,
        "Errors": errs,
        "Warnings": warns
    })