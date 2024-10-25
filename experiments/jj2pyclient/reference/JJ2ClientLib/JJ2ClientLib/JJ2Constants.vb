Namespace JJ2
    Public Enum JJ2_Character
        JAZZ = 0
        SPAZ = 1
        LORI = 2
        BIRD = 3
        FROG = 4
    End Enum
    Public Enum JJ2_Player_Team
        BLUE
        RED
        GREEN
        YELLOW
    End Enum
    Public Enum SFSDF
        aPLAYERBULLETP5
    End Enum
    Public Enum JJ2_DIRECTION

        UP = 8
    End Enum
    Public Enum JJ2_Disconnect_Message As Byte
        DISCONNECT_MESSAGE_SERVER_FULL = &H1
        DISCONNECT_MESSAGE_VERSION_DIFFERENT = &H2
        DISCONNECT_MESSAGE_ERROR_DURING_HANDSHAKING = &H4
        DISCONNECT_MESSAGE_FEATURE_NOT_SUPPORTED_IN_SHAREWARE = &H5
        DISCONNECT_MESSAGE_WINSOCK_ERROR = &H8
        DISCONNECT_MESSAGE_DENIED = &HD
        DISCONNECT_MESSAGE_ERROR_DOWNLOADING_LEVEL = &H6
        DISCONNECT_MESSAGE_CONNECTION_LOST = &H7
        DISCONNECT_MESSAGE_CONNECTION_TIMED_OUT = &H9
        DISCONNECT_MESSAGE_SERVER_STOPPED = &HA
        DISCONNECT_MESSAGE_KICKED_OFF = &HB
        DISCONNECT_MESSAGE_BANNED = &HC
        DISCONNECT_MESSAGE_VERSION_OF_JJ2PLUS_IS_DIFFERENT = &HE
        DISCONNECT_MESSAGE_SERVER_KICKED_YOU_FOR_IDLING = &HF
        DISCONNECT_MESSAGE_NO_DOWNLOADS_ALLOWED = &H10
        DISCONNECT_MESSAGE_UNAUTHORIZED_FILE_REQUEST = &H11
        DISCONNECT_MESSAGE_NO_SPLITSCREENERS_ALLOWED = &H12
    End Enum
    Public Enum JJ2Plus_Game_State As Byte
        NORMAL
        TIME_LIMIT
        PREGAME
        OVERTIME
    End Enum
    Public Enum JJ2_Game_Type
        SINGLE_PLAYER
        COOP
        BATTLE
        RACE
        TREASURE
        CAPTURE
    End Enum
    Public Enum JJ2_Custom_Game_Type
        ' {"", "Roast Tag", "LRS", "XLRS", "Pestilence", "", "", "", "", "", "", "Team Battle", "Jailbreak", "Death CTF", "Flag Run", "TLRS", "Domination", "Head Hunters"}
        NONE = 0
        RT = 1
        LRS = 2
        XLRS = 3
        PEST = 4
        TB = 11
        JB = 12
        DCTF = 13
        FR = 14
        TLRS = 15
        DOM = 16
        HEAD = 17
    End Enum
    Public Enum Network_Protocol
        sckTCPProtocol = 0
        sckUDPProtocol = 1
    End Enum
End Namespace
