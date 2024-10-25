Namespace JJ2
    Public Class JJ2SocketInfo 'or client
        Const MAXSPLITSCREENPLAYERS = 8
        Public Property NumOfPlayers As Byte = 0
        Public Property Plus As Boolean = False
        Public Property Active As Boolean 'not connecting (c...) or downloading
        Public Property Idle As Boolean
        Public Property Downloading As Boolean
        Public Property Admin As Boolean = 0
        Public Property Spectating As Boolean = 0
        Public Property AntiSpamCount As UShort = 0
        Public PlayerID(MAXSPLITSCREENPLAYERS - 1) As Byte
        Public JoinDate As Date
        Public IP As String = ""
        Public Sub reset()
            NumOfPlayers = 0
            For i As Byte = 0 To playerID.Length - 1
                playerID(i) = &HFF
            Next
            Idle = 0
            Admin = 0
            Spectating = 0
            AntiSpamCount = 0
            Active = True 'always true for vanilla? or mby false and set true by receiving packet 0x02
            IP = ""
        End Sub
        Sub New()
            For i = 0 To playerID.Length - 1
                playerID(i) = &HFF
            Next
        End Sub
    End Class

    Public Class JJ2Player
        Public Property ClientID As Byte = &HFF
        Public Property Client As JJ2SocketInfo = Nothing
        Public Property IsOut As Boolean
        Public Property Character As JJ2_Character = 0
        Public Property Team As Byte = &HFF
        Public Property Name As String = ""
        Public Property Latency As Int16 = -1
        Public Color(3) As Byte
        Public Property Health As Byte = 0
        Public Property Gems As Integer = 0
        Public Property Roasts As Integer = 0
        Public Property Deaths As Integer = -1
        Public Property SelectedGun As Integer 'firetype
        Public GunAmmo(9) As Integer
        Public GunPower(9) As Byte
        Public Property ShieldType As Integer = 0
        Public Sub reset(Optional plusServer As Boolean = False)
            ClientID = &HFF
            Client = Nothing
            Team = &HFF
            latency = -1
            Roasts = 0
            SelectedGun = 0
            ShieldType = 0
            For i = 0 To GunPower.Length - 1
                GunAmmo(i) = 0
                GunPower(i) = 0
            Next
            IsOut = False
            If plusServer = False Then
                Deaths = -1
            Else
                Deaths = 0
            End If
        End Sub
        Public Sub ClearStats(Optional plusServer As Boolean = False)
            Roasts = 0
            If plusServer = False Then Deaths = -1 Else Deaths = 0
        End Sub
        Public Sub Update(ByVal playersocketIndex As Byte, ByVal playerCharacter As Byte, ByVal playerTeam As Byte, ByVal client As JJ2SocketInfo)
            ClientID = playersocketIndex
            Character = playerCharacter
            Team = playerTeam
            Me.Client = client
        End Sub
        Sub New(ByVal playerSocketIndex As Byte, ByVal playerCharacter As Byte, ByVal playerTeam As Byte, ByVal client As JJ2SocketInfo)
            ClientID = playerSocketIndex
            Character = playerCharacter
            Team = playerTeam
            Me.Client = client
        End Sub
        Public ReadOnly Property Active As Boolean
            Get
                Return Client.Active
            End Get
        End Property
        Public ReadOnly Property Idle As Boolean
            Get
                Return Client.Idle
            End Get
        End Property
        Public ReadOnly Property Downloading As Boolean
            Get
                Return Client.Downloading
            End Get
        End Property
        Public ReadOnly Property Spectating As Boolean
            Get
                Return Client.Spectating
            End Get
        End Property
    End Class
    Public Structure JJ2Team
        Public Property Enabled As Boolean
        Public Property Color As JJ2_Player_Team
        Public Property Score As Integer

        Public Property FlagCarriedByPlayerID As Byte

        Dim _flagIsCaptured As Boolean
        Public Property FlagIsCaptured As Boolean
            Get
                Return _flagIsCaptured
            End Get
            Set(value As Boolean)
                _flagIsCaptured = value
                If Not value Then
                    FlagCarriedByPlayerID = &HFF
                End If
            End Set
        End Property
        Sub Reset()
            Score = 0
            FlagIsCaptured = False
        End Sub
    End Structure
    Public Class JJ2PlusGameSettings
        '----- set by plus packet (0x3F)
        Public Property PlusOnly As Boolean
        Public Property FriendlyFire As Boolean
        Public Property NoMovement As Boolean
        Public Property NoBliking As Boolean
        Public Property ReadyCommandEnabled As Boolean
        Public Property FireBall As Boolean
        Public Property WallJumping As Boolean

        '----- set by other packets
        Public Property MaxResolutionWidth As UShort = 640
        Public Property MaxResolutionHeight As UShort = 480
    End Class
End Namespace
