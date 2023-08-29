class JJ2SocketInfo:
    MAXSPLITSCREENPLAYERS = 8

    def __init__(self):
        self.NumOfPlayers = 0
        self.Plus = False
        self.Active = False
        self.Idle = False
        self.Downloading = False
        self.Admin = False
        self.Spectating = False
        self.AntiSpamCount = 0
        self.PlayerID = [0xFF] * self.MAXSPLITSCREENPLAYERS
        self.JoinDate = None
        self.IP = ""

    def reset(self):
        self.NumOfPlayers = 0
        self.PlayerID = [0xFF] * self.MAXSPLITSCREENPLAYERS
        self.Idle = False
        self.Admin = False
        self.Spectating = False
        self.AntiSpamCount = 0
        self.Active = True
        self.IP = ""

class JJ2Player:
    def __init__(self):
        self.ClientID = 0xFF
        self.Client: JJ2Client = None
        self.IsOut = False
        self.Character = 0
        self.Team = 0xFF
        self.Name = ""
        self.Latency = -1
        self.Color = [0] * 3
        self.Health = 0
        self.Gems = 0
        self.Roasts = 0
        self.Deaths = -1
        self.SelectedGun = 0
        self.GunAmmo = [0] * 10
        self.GunPower = [0] * 10
        self.ShieldType = 0

    def reset(self, plusServer=False):
        self.ClientID = 0xFF
        self.Client = None
        self.Team = 0xFF
        self.Latency = -1
        self.Roasts = 0
        self.SelectedGun = 0
        self.ShieldType = 0
        self.GunAmmo = [0] * 10
        self.GunPower = [0] * 10
        self.IsOut = False
        self.Deaths = -1 if not plusServer else 0

    def clear_stats(self, plusServer=False):
        self.Roasts = 0
        self.Deaths = -1 if not plusServer else 0

    def update(self, playersocketIndex, playerCharacter, playerTeam, client):
        self.ClientID = playersocketIndex
        self.Character = playerCharacter
        self.Team = playerTeam
        self.Client = client

    @property
    def active(self):
        return self.Client.Active

    @property
    def idle(self):
        return self.Client.Idle

    @property
    def downloading(self):
        return self.Client.Downloading

    @property
    def spectating(self):
        return self.Client.Spectating

class JJ2Team:
    def __init__(self):
        self.Enabled = False
        self.Color = 0
        self.Score = 0
        self.FlagCarriedByPlayerID = 0xFF
        self._flagIsCaptured = False

    @property
    def flag_is_captured(self):
        return self._flagIsCaptured

    @flag_is_captured.setter
    def flag_is_captured(self, value):
        self._flagIsCaptured = value
        if not value:
            self.FlagCarriedByPlayerID = 0xFF

    def reset(self):
        self.Score = 0
        self.flag_is_captured = False

class JJ2PlusGameSettings:
    def __init__(self):
        self.PlusOnly = False
        self.FriendlyFire = False
        self.NoMovement = False
        self.NoBlinking = False
        self.ReadyCommandEnabled = False
        self.FireBall = False
        self.WallJumping = False
        self.MaxResolutionWidth = 640
        self.MaxResolutionHeight = 480
