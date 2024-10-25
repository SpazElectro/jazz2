'//By Necrolyte, https://github.com/Mustafa1177/JJ2ClientLib
Imports System.Net
Imports System.Net.Sockets
Imports System.Text
Namespace JJ2
    'The library
    Public Class JJ2Client
        Public Property UserData As Object
        Dim _name As String = "(NoName)"
        Dim _char As Byte
        Dim _team As Byte

        Dim Winsock1 As Socket
        Dim Winsock2 As UdpClient
        Dim _connected As Boolean = False
        Dim _serverAddress As String = ""
        Dim _serverIPAddress As String = ""
        Dim _serverPort As UShort = 10052
        Dim _remoteEP As IPEndPoint
        Dim socketID As Byte = &HFF
        Dim _ID As Byte = &HFF
        Dim _packet0x0EWasSent As Boolean = False
        Dim _gameType As Byte
        Dim _maxScore As Integer

        Const _connectionlimit = 64
        Public ActiveClients(_connectionlimit) As Boolean
        Public JJ2ClientsSockInfo(_connectionlimit) As JJ2SocketInfo
        Public Players(254) As JJ2Player
        Public Teams(4 - 1) As JJ2Team
        Dim _plusServer As Boolean
        Dim _idleServerMode As Boolean
        Dim _specialServer As Boolean
        Dim _currentLevelName As String = ""
        Dim _nextLevelName As String = ""
        Dim _cycling As Boolean = False
        Dim _isFirstScoreUpdate As Boolean = True

        Dim WithEvents every1SecTimer As New Timers.Timer
        Dim WithEvents AnimTimer As New Timers.Timer
        Dim udpTimerState As Boolean = False
        Dim ConnectionTimeOut As Byte = 30
        Dim UDPCount As Byte = &H3

        'Options and extra values
        Public Property JJ2Version As String = ""
        Public Property Plus As Byte = 1
        Public Property PlusVersion As Integer = &H50012
        Public Property _AutoSpec As Byte = 1
        Public Property ExtraLatency As Int16 = 0
        Public Property AntiSpam As Boolean = False
        Dim _antiSpamClearSeconds As Byte = 2
        Dim _antiSpamNumOfMsgsToKick As UShort = 20
        Dim _levelCRC32 As Int32
        Dim _tilesetCRC32 As Int32
        Public Property TotalBytesRecv As ULong = 0
        Public Property TotalBytesSent As ULong = 0

        'JJ2+ Variables
        Dim _customGameMode As Byte = 0
        Public Property PlusGameSettings As New JJ2PlusGameSettings
        Public scriptModules As New Dictionary(Of String, Byte) 'Name, Id
        Public scriptsRequiredFiles As New List(Of String)
        Dim _serverPlusVersion As Integer = &H0
        Dim _scriptsEnabled As Boolean = False
        Dim _plusOnly As Boolean
        Dim _gameState As JJ2Plus_Game_State = 0
        Dim _gameStarted As Boolean = True
        Dim _gameInProgress As Boolean = False
        Dim _isFirstGameState As Boolean = True

        Public Property TimeLimit As Integer = 0 'in ms
        Public Property TimeRemaining As Integer = 0 'in ms
        Public Property timerInfoUpdateDate As Date



        '---------------------------- Client Events ----------------------------'
        Public Event Connected_Event(ByVal serverIPAddrees As String, ByVal serverAddress As String, ByVal serverPort As UShort, ByVal user As Object)
        Public Event Failed_To_Connect_Event(ByVal serverAddress As String, ByVal serverPort As UShort, ByVal user As Object)
        Public Event Recveived_Server_Data_Event(ByVal socketIndex As Byte, ByVal PlayerID As Byte, ByVal levelName As String, ByVal gameMode As Byte, ByVal maxScores As Byte, ByVal plusServer As Boolean, ByVal user As Object)
        Public Event Joined_Event(ByVal socketIndex As Byte, ByVal PlayerID As Byte, ByVal serverIPAddrees As String, ByVal serverAddress As String, ByVal serverPort As UShort, ByVal user As Object)
        Public Event Disconnected_Event(ByVal disconnectMessage As JJ2_Disconnect_Message, ByVal serverIPAddrees As String, ByVal serverAddress As String, ByVal serverPort As UShort, ByVal user As Object)
        Public Event Client_Connected_Event(ByVal connectedSocketIndex As Byte, ByVal numberOfPlayersFromClient As Byte, ByVal user As Object)
        Public Event Client_Disconnected_Event(ByVal disconnectedSocketIndex As Byte, ByVal disconnectMessage As JJ2_Disconnect_Message, ByVal numberOfPlayersFromClient As Byte, ByVal user As Object)
        Public Event Players_List_Update_Event(ByVal updatedPlayersIDs As Byte(), ByVal updatedClientsIndices As Byte(), ByVal user As Object)
        Public Event Player_Joined_Event(ByVal playerName As String, ByVal playerID As Byte, ByVal socketIndex As Byte, ByVal character As Byte, ByVal team As Byte, ByVal user As Object)
        Public Event Player_Left_Event(ByVal playerName As String, ByVal disconnectMessage As JJ2_Disconnect_Message, ByVal playerID As Byte, ByVal socketIndex As Byte, ByVal user As Object)
        Public Event Message_Received_Event(ByVal msg As String, ByVal playerName As String, ByVal team As Byte, ByVal playerID As Byte, ByVal playerSocketIndex As Byte, ByVal user As Object)
        Public Event Console_Message_Recveived_Event(ByVal msg As String, ByVal msgType As Byte, ByVal user As Object)
        Public Event Level_Initialized_Event(ByVal levelName As String, ByVal yourName As String, ByVal yourID As Byte, ByVal yourSocketIndex As Byte, ByVal user As Object)
        Public Event End_Of_Level_Event(winnerID As Byte, winnerScore As Integer, playersIDs As Byte(), playersPlaces As Byte(), teamsIDs As Byte(), teamsPlaces As Byte(), ByVal user As Object)
        Public Event Idle_Server_Mode_Update_Event(ByVal idleServerModeState As Boolean, ByVal user As Object)
        Public Event Latency_Update_Event(ByVal user As Object)
        Public Event Clients_State_Update_Event(ByVal user As Object)
        Public Event Client_Spectate_Event(ByVal spectatorMode As Boolean, ByVal socketIndex As Byte, ByVal user As Object)
        Public Event Player_Spectate_Event(ByVal spectatorModeState As Boolean, ByVal playerID As Byte, ByVal socketIndex As Byte, ByVal user As Object)
        Public Event Remote_Admins_Update_Event(ByVal user As Object)
        Public Event Game_State_Changed_Event(ByVal gameStarted As Boolean, ByVal gameWasStarted As Boolean, ByVal newGameState As JJ2Plus_Game_State, timeRemaining As Integer, timeLimit As Integer, newGame As Boolean, firstTime As Boolean, ByVal user As Object)
        Public Event JJ2_Plus_Network_Stream_Data_Arrival(ByVal packet As Byte(), sourceId As Byte)
        Public Event Max_Resolution_Set_Event(ByVal maxWidth As UShort, ByVal maxHeight As UShort, ByVal user As Object)
        Public Event Team_State_Change_Event(team As Byte, enabled As Boolean, user As Object)
        Public Event Game_Settings_Update_Event(gameMode As JJ2_Game_Type, customGameMode As JJ2_Custom_Game_Type, maxScore As Integer, user As Object)
        Public Event Custom_IP_Update_Event(ByVal user As Object)
        Public Event Error_Event(ByVal disconnected As Boolean, ByVal errorCode As Integer, ByVal errorMsg As String, ByVal user As Object)
        'Gameplay events [TCP]
        Public Event Gameplay_Player_Stats_List_Update_Event(user As Object)
        Public Event Gameplay_Teams_Scores_Set_Event(teamsUpdated As Byte(), user As Object)
        Public Event Gameplay_Team_Score_Set_Event(team As Byte, oldScore As Integer, newScore As Integer, user As Object)
        Public Event Gameplay_Player_Deaths_Update_Event(playerID As Byte, deaths As Integer, user As Object)
        'Gameplay events [UDP]
        Public Event Gameplay_Player_Roast_Event(victimID As Byte, victimKills As Integer, killerID As Byte, killerKills As Integer, user As Object)
        Public Event Gameplay_Player_Hit_Event(victimID As Byte, victimHealth As Byte, attackerID As Byte, user As Object)
        Public Event Gameplay_Teams_Scores_Update_Event(teamsUpdated As Byte(), user As Object)
        Public Event Gameplay_Team_Score_Update_Event(team As Byte, oldScore As Integer, newScore As Integer, user As Object)
        Public Event Gameplay_Team_Scored_Event(team As Byte, oldScore As Integer, newScore As Integer, user As Object)
        Public Event Gameplay_CTF_Flags_Update_Event(teamFlagsUpdated As Byte(), user As Object)
        Public Event Gameplay_CTF_Flag_Update_Event(team As Byte, flagIsCaptured As Boolean, carrierID As Byte, user As Object)
        Public Event Gameplay_Plus_Bullet_Shoot_Event(gun As Byte, power As Boolean, bulletX As Short, bulletY As Short, bulletVx As SByte, bulletVy As SByte, bulletAngle As SByte, playerVx As SByte, lifetime As Byte, ammoRemaining As Byte, direction As SByte, vertical As Boolean, playerID As Byte, friendly As Boolean, user As Object)


        '---------------------------- Client Functions ----------------------------'
        Public Function JoinServer(ByVal serverAddress As String, ByVal user As Object, Optional ByVal name As String = "", Optional ByVal port As UShort = 10052) As String
            'Check if arguments are valid
            If name.Length > 20 Then
                Return "Invalid name length."
            End If
            If port = 0 Then
                Return "Invalid port number."
            End If

            'Store given args
            UserData = user
            If name <> "" Then
                _name = name
            End If
            _serverAddress = serverAddress
            _serverPort = port
            _connected = False

            'Init sockets
            If Winsock1 IsNot Nothing Then
                Winsock1.Close()
            End If

            Winsock1 = New Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
            If Winsock2 Is Nothing Then
                Winsock2 = New UdpClient(0)
            End If

            'initialize timers
            RemoveHandler every1SecTimer.Elapsed, AddressOf every1SecTimerTick
            every1SecTimer.Stop()
            AddHandler every1SecTimer.Elapsed, AddressOf every1SecTimerTick
            every1SecTimer.Interval = 1000
            every1SecTimer.Start()

            RemoveHandler AnimTimer.Elapsed, AddressOf AnimTimerTick
            AnimTimer.Stop()
            AddHandler AnimTimer.Elapsed, AddressOf AnimTimerTick
            AnimTimer.Interval = 125
            '    AnimTimer.Start()

            'Connect
            Winsock1.BeginConnect(serverAddress, port, New AsyncCallback(AddressOf Winsock1_Connect_Event), Nothing)
            Return ""
        End Function

        Public Function Leave() As String
            WinsockClose()
        End Function
        Sub New()
            For i = 0 To 32 - 1
                Players(i) = New JJ2Player(&HFF, 0, 0, Nothing)
            Next
            For i = 0 To Teams.Length - 1
                Teams(i) = New JJ2Team With {.Color = i, .FlagCarriedByPlayerID = &HFF}
            Next
        End Sub
        Public Sub Dispose()
            RemoveHandler every1SecTimer.Elapsed, AddressOf every1SecTimerTick
            every1SecTimer.Dispose()
            AnimTimer.Dispose()
            If Winsock1 IsNot Nothing Then
                Winsock1.Close()
            End If
            If Winsock2 IsNot Nothing Then
                Winsock2.Close()
            End If
        End Sub

        Dim IdleServerPacket As Byte() = {&H42, &HC0, &H2, &H46, &H2, &H27, &H0, &HCC}
        Dim _joiningData1 As Byte() = {&H9, &HF, &H0, &H0, &H32, &H34, &H20, &H20, &H1}
        Private Sub Winsock1_Connect_Event(ByVal ar As IAsyncResult) 'onConnect
            Try
                Winsock1.EndConnect(ar)
                _connected = True
                socketID = &HFF
                _idleServerMode = False
                _plusServer = CBool(Plus)
                _serverPlusVersion = 0
                PlusGameSettings = New JJ2PlusGameSettings
                _packet0x0EWasSent = False
                Dim ipPort As String() = Winsock1.RemoteEndPoint.ToString.Split(":")
                _remoteEP = New IPEndPoint(IPAddress.Parse(ipPort(0)), CInt(ipPort(1)))
                _serverIPAddress = ipPort(0)

                If Winsock2 Is Nothing Then
                    Winsock2 = New UdpClient(0)
                End If
                Reset()
                RaiseEvent Connected_Event(ipPort(0), _serverAddress, ipPort(1), UserData)

                Array.Copy(BitConverter.GetBytes(CUShort(Winsock2.Client.LocalEndPoint.ToString.Split(":")(1))), 0, _joiningData1, 2, 2)
                'assign local port of UDP socket

                TotalBytesRecv = 0
                TotalBytesSent = 0

                Winsock2.Client.BeginReceiveFrom(BufferUDP, 0, BufferUDP.Length, SocketFlags.None, _remoteEP, New AsyncCallback(AddressOf Winsock2_DataArrival), _remoteEP)

                Winsock1.BeginReceive(BufferTCP, 0, BufferTCP.Length, SocketFlags.None, New AsyncCallback(AddressOf Winsock1_DataArrival), Nothing)
                If JJ2Version <> "" Then
                    Dim versionBytes As Byte() = Encoding.ASCII.GetBytes(JJ2Version)
                    If versionBytes.Length <= 4 Then
                        Array.Copy(versionBytes, 0, _joiningData1, 4, versionBytes.Length)
                    End If
                End If
                Winsock1SendData(_joiningData1)
            Catch argEx As ArgumentException
            Catch obDisEx As ObjectDisposedException
            Catch sockEx As SocketException
                WinsockClose(255)
            End Try
        End Sub
        Private Sub WinsockClose(Optional ByVal disconType As Byte = 0)
            If Winsock1 IsNot Nothing Then
                Winsock1.Close()
            End If

            If _connected Then
                _connected = False
                _packet0x0EWasSent = False
                If disconType <> &HFF Then
                    RaiseEvent Disconnected_Event(disconType, _serverIPAddress, _serverAddress, _serverPort, UserData)
                End If
                _serverIPAddress = ""
            End If
            If disconType = &HFF Then
                RaiseEvent Failed_To_Connect_Event(_serverAddress, _serverPort, UserData)
            End If
        End Sub
        Private Sub Reset()
            For i As Byte = 0 To _connectionlimit
                ActiveClients(i) = False
                If JJ2ClientsSockInfo(i) IsNot Nothing Then
                    JJ2ClientsSockInfo(i).reset()
                End If
            Next
            For i2 As Byte = 0 To Players.Length - 1
                If Players(i2) IsNot Nothing Then
                    Players(i2).reset(PlusServer)
                End If
            Next
            For i = 0 To Teams.Length - 1
                Teams(i).Enabled = False
                Teams(i).Reset()
            Next
            Teams(0).Enabled = True : Teams(1).Enabled = True
            PlusGameSettings = New JJ2PlusGameSettings
            _plusOnly = False
            _serverPlusVersion = 0
            _scriptsEnabled = False
            scriptsRequiredFiles.Clear()
            scriptModules.Clear()

            _isFirstGameState = True
            _gameInProgress = False
            _specialServer = False
            _ID = &HFF
            socketID = &HFF
        End Sub

        Dim PlusCheck(3) As Byte
        Dim CheckDatafrom10for9(3) As Byte
        Dim joiningData2Plus As Byte() = {&H8, &H3F, &H20, &H3, &H9, &H0, &H5, &H0}

        Public Property DefaultEncoding As System.Text.Encoding = System.Text.Encoding.Default
        Public Property DefaultNameEncoding As System.Text.Encoding = System.Text.Encoding.ASCII
        Dim BufferTCP(1024 * 10 - 1) As Byte
        Private Sub Winsock1_DataArrival(ByVal ar As IAsyncResult)
            Try
                Dim dataLength As Integer = Winsock1.EndReceive(ar)
                Dim recv(dataLength - 1) As Byte
                Array.Copy(BufferTCP, recv, dataLength)
                If _plusServer Then
                    Winsock1_DataArrival_Read_PLUS(recv)
                Else
                    Winsock1_DataArrival_Read(recv)
                End If
                TotalBytesRecv += dataLength
                Winsock1.BeginReceive(BufferTCP, 0, BufferTCP.Length, SocketFlags.None, New AsyncCallback(AddressOf Winsock1_DataArrival), Nothing)
            Catch nullEx As NullReferenceException
            Catch obDisEx As ObjectDisposedException
            Catch argEx As ArgumentException
                '    WinsockClose(&H0)'''''''''''''
            Catch sockEx As SocketException
                WinsockClose(&H7)
            End Try
        End Sub
        Private Sub Winsock1_DataArrival_Read_PLUS(ByVal recv As Byte())
            ''''''''Try
            If recv.Length > 2 Then
                    Dim packetLength As UInteger = 0
                    Dim packetRealLength As UInteger = 0
                    Dim packetContentLength As UInteger 'exclude length byte(packet length from packetID to the end)
                    Dim Starting As UInteger = 0
                    Dim addnmbr As Integer = 0
                    Dim packetID As Byte = &H0
                    Dim packetNum As Integer = 0
                    Dim readingComplete As Boolean = False
                    Dim packStartingIndex As Integer = 0
                    While readingComplete = False
                        '    If recv.Length > Starting + addnmbr + 1 Then '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
                        If Not (Starting + 0) < recv.Length Then 'new thing prevents OutOfRange reading
                            Exit While
                        End If
                        If recv(Starting + 0) = 0 Then
                            If Starting + 1 = recv.Length Then
                                Exit While
                            ElseIf Starting + 1 < recv.Length Then
                                If recv(Starting + 1) = 0 Then
                                    Exit While
                                End If
                            End If
                            Dim packetLengthInBytes As Byte() = {recv(Starting + 1), recv(Starting + 2)}
                            packetLength = BitConverter.ToUInt16(packetLengthInBytes, 0)
                            packetRealLength = packetLength + 3
                            packetContentLength = packetLength
                            addnmbr = 2
                            If packetRealLength > recv.Length Then
                                Exit While
                            End If
                        End If
                        If recv(Starting + 0) <> 0 Then
                            If recv(Starting) > (recv.Length - Starting) Then
                                Exit While
                            End If
                            packetLength = recv(Starting + 0)
                            packetRealLength = packetLength
                            packetContentLength = packetLength - 1
                            addnmbr = 0
                        End If
                        packStartingIndex = Starting + addnmbr
                        packetID = recv(packStartingIndex + 1)


                        '''''''''''''

                        Select Case packetID
                            Case &H1B 'Chat message
                                Dim senderSocketID As Byte = recv(packStartingIndex + 2)
                                Dim team As Byte = recv(packStartingIndex + 3)
                                Dim message As String = System.Text.Encoding.Default.GetString(recv, packStartingIndex + 4, packetLength - 3)
                                Dim senderPlayerID As Byte = JJ2ClientsSockInfo(senderSocketID).playerID(0)
                                If senderPlayerID <> &HFF Then
                                    RaiseEvent Message_Received_Event(message, Players(senderPlayerID).Name, team, senderPlayerID, senderSocketID, UserData)
                                End If

                                'Anti-spam system
                                If AntiSpam <> False Then
                                    If senderSocketID < JJ2ClientsSockInfo.Length And JJ2ClientsSockInfo(senderSocketID) IsNot Nothing Then
                                        If JJ2ClientsSockInfo(senderSocketID).AntiSpamCount < JJ2ClientsSockInfo(senderSocketID).AntiSpamCount.MaxValue Then
                                            JJ2ClientsSockInfo(senderSocketID).AntiSpamCount += 1
                                        End If

                                        If JJ2ClientsSockInfo(senderSocketID).AntiSpamCount >= _antiSpamNumOfMsgsToKick Then
                                            SendMessage("/kick " & (JJ2ClientsSockInfo(senderSocketID).playerID(0) + 1) & " |(auto) |fuck off")
                                            JJ2ClientsSockInfo(senderSocketID).AntiSpamCount = 0
                                        End If
                                    End If
                                End If

                            Case &HD 'Player left
                                Dim disconnectMsg As Byte = recv(packStartingIndex + 2)
                                Dim leftClientSocketIndex As Byte = recv(packStartingIndex + 3)
                                If leftClientSocketIndex <> socketID Then
                                    If leftClientSocketIndex < JJ2ClientsSockInfo.Length Then
                                        ActiveClients(leftClientSocketIndex) = False
                                        If JJ2ClientsSockInfo(leftClientSocketIndex) IsNot Nothing Then
                                            If CBool(JJ2ClientsSockInfo(leftClientSocketIndex).NumOfPlayers) Then
                                                RaiseEvent Client_Disconnected_Event(leftClientSocketIndex, disconnectMsg, JJ2ClientsSockInfo(leftClientSocketIndex).NumOfPlayers, UserData)
                                                For wokingOnPlayer As Byte = 0 To JJ2ClientsSockInfo(leftClientSocketIndex).NumOfPlayers - 1
                                                    If JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) <> &HFF Then
                                                        If Players(JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)) IsNot Nothing Then
                                                            If JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) <> &HFF And JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) < Players.Length Then
                                                                Dim playerName As String = Players(JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)).Name
                                                                Dim playerID As Byte = JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)
                                                                JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) = &HFF
                                                                If JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) <> &HFF Then
                                                                    Players(JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)).reset()
                                                                End If
                                                                RaiseEvent Player_Left_Event(playerName, disconnectMsg, playerID, leftClientSocketIndex, UserData)
                                                            End If
                                                        End If
                                                    End If
                                                Next
                                                JJ2ClientsSockInfo(leftClientSocketIndex).reset()
                                            End If
                                        End If
                                    End If
                                Else
                                    'You 
                                    WinsockClose(disconnectMsg)
                                End If

                            Case &H10 'Info
                                socketID = recv(packStartingIndex + 2)
                                Dim levelNameLength As Byte = recv(packStartingIndex + 4)
                                socketID = recv(packStartingIndex + 2)
                                _ID = recv(packStartingIndex + 3)
                                _nextLevelName = Encoding.ASCII.GetString(recv).Substring(packStartingIndex + 5, levelNameLength)
                                _gameType = recv(packStartingIndex + levelNameLength + 13)
                                _maxScore = recv(packStartingIndex + levelNameLength + 14)
                                If recv(packStartingIndex) <= (15 + levelNameLength) Then
                                    'Non-plus server
                                    _plusServer = False
                                    AnimTimer.Start()
                                    socketID = recv(Starting + addnmbr + 2)
                                    Dim joiningData2Part1 As Byte() = {0, &HE, &H1, &H1, &H1, &H18, &H20, &H28, &H11} '{&H19, &HE, &H1, &H1, &H0, &H1, &H10, &H18, &H20, &H28, &H11, &H1, &HA, &HD, &H0, &H0, &H52, &H65, &H63, &H6F, &H72, &H64, &H65, &H72, &H0}
                                    Dim joiningData2Part2 As Byte() = DefaultNameEncoding.GetBytes(_name)
                                    Dim joiningData2(joiningData2Part1.Length + joiningData2Part2.Length) As Byte
                                    Array.Copy(joiningData2Part1, joiningData2, joiningData2Part1.Length)
                                    Array.Copy(joiningData2Part2, 0, joiningData2, joiningData2Part1.Length, joiningData2Part2.Length)
                                    joiningData2(0) = joiningData2.Length
                                    If Not _packet0x0EWasSent Then
                                        _packet0x0EWasSent = True
                                        If Plus <> 0 Then
                                            Winsock1SendData(joiningData2Plus)
                                        End If
                                        Winsock1SendData(joiningData2)
                                    End If
                                Else
                                    'Plus server
                                    _plusServer = True
                                    AnimTimer.Stop()
                                    Dim joiningData3PlusPart1 As Byte() = {&H19, &HE, &H1, &H1, 0, &H1, &H10, &H18, &H20, &H28, &H11, &H1, &HA, &HD, &H0, &H0} '{&H19, &HE, &H1, &H1, &H0, &H1, &H10, &H18, &H20, &H28, &H11, &H1, &HA, &HD, &H0, &H0, &H52, &H65, &H63, &H6F, &H72, &H64, &H65, &H72, &H0}
                                    Dim joiningData3PlusPart2 As Byte() = DefaultNameEncoding.GetBytes(_name)
                                    Dim joiningData3Plus(joiningData3PlusPart1.Length + joiningData3PlusPart2.Length) As Byte
                                    Array.Copy(joiningData3PlusPart1, joiningData3Plus, joiningData3PlusPart1.Length)
                                    Array.Copy(joiningData3PlusPart2, 0, joiningData3Plus, joiningData3PlusPart1.Length, joiningData3PlusPart2.Length)
                                    joiningData3Plus(0) = joiningData3Plus.Length
                                    _nextLevelName = Encoding.ASCII.GetString(recv).Substring(packStartingIndex + 5, levelNameLength)

                                    PlusCheck(0) = recv(packStartingIndex + 16 + levelNameLength - 1)
                                    PlusCheck(1) = recv(packStartingIndex + 16 + levelNameLength)
                                    PlusCheck(2) = recv(packStartingIndex + 16 + levelNameLength + 1)
                                    PlusCheck(3) = recv(packStartingIndex + 16 + levelNameLength + 2)
                                    CheckDatafrom10for9(0) = recv(packStartingIndex + 20 + levelNameLength - 1)
                                    CheckDatafrom10for9(1) = recv(packStartingIndex + 20 + levelNameLength)
                                    CheckDatafrom10for9(2) = recv(packStartingIndex + 20 + levelNameLength + 1)
                                    CheckDatafrom10for9(3) = recv(packStartingIndex + 20 + levelNameLength + 2)
                                    Dim UDPPacket As Byte() = {&HA, &H15, &H9, &H0}
                                    Winsock2SendData(UDPPacket)
                                    Array.Copy(BitConverter.GetBytes(PlusVersion), 0, joiningData2Plus, 4, 4) 'Write your plus version to packet
                                    Winsock1SendData(joiningData2Plus)
                                    If Not _packet0x0EWasSent Then
                                        _packet0x0EWasSent = True
                                        Winsock1SendData(joiningData3Plus)
                                    End If
                                End If
                                RaiseEvent Recveived_Server_Data_Event(socketID, _ID, _currentLevelName, _gameType, _maxScore, _plusServer, UserData)
                            Case &H11 'Player joined
                                Dim joinedClientSocketIndex As Byte = recv(packStartingIndex + 2)
                                Dim numIfJoinedPlayers As Byte = recv(packStartingIndex + 3)
                                Dim playerArrStartingIndex As UInt16 = packStartingIndex + 4
                                If joinedClientSocketIndex < _connectionlimit Then
                                    'init socket info
                                    If JJ2ClientsSockInfo(joinedClientSocketIndex) Is Nothing Then
                                        JJ2ClientsSockInfo(joinedClientSocketIndex) = New JJ2SocketInfo
                                    Else
                                        JJ2ClientsSockInfo(joinedClientSocketIndex).reset()
                                    End If

                                    For workingOnPlayer As Byte = 0 To numIfJoinedPlayers - 1
                                        Dim playerNumber As Byte = recv(playerArrStartingIndex)
                                        Dim charTeam As Byte = recv(playerArrStartingIndex + 1)

                                        If workingOnPlayer >= JJ2ClientsSockInfo(joinedClientSocketIndex).playerID.Length Then
                                            Exit For 'Very important, this might causes problems in players of server!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                        End If

                                        If joinedClientSocketIndex < JJ2ClientsSockInfo.Length And playerNumber < Players.Length Then
                                            JJ2ClientsSockInfo(joinedClientSocketIndex).playerID(workingOnPlayer) = playerNumber
                                            Dim fdgfdgdfgdfgfdg As Byte = JJ2ClientsSockInfo(joinedClientSocketIndex).playerID(workingOnPlayer)

                                            'init player
                                            If Players(playerNumber) Is Nothing Then
                                                Players(playerNumber) = New JJ2Player(joinedClientSocketIndex, charTeam Mod &H10, Math.Floor(charTeam / &H10), JJ2ClientsSockInfo(joinedClientSocketIndex))
                                            Else
                                                Players(playerNumber).reset()
                                                Players(playerNumber).Update(joinedClientSocketIndex, charTeam Mod &H10, Math.Floor(charTeam / &H10), JJ2ClientsSockInfo(joinedClientSocketIndex))
                                                JJ2ClientsSockInfo(joinedClientSocketIndex).Active = False 'make it true whenever u recv udp packet 0x02 or until tcp packet 0x44 tells u to do so.
                                            End If
                                        Array.Copy(recv, playerArrStartingIndex + 3, Players(playerNumber).Color, 0, 4)

                                        Dim playerNameLength As Byte = 0
                                            Dim whileHelper As UShort = playerArrStartingIndex + 13
                                            While recv(whileHelper) <> &H0
                                                playerNameLength += 1
                                                whileHelper += 1
                                                If recv.Length <= whileHelper Then
                                                    Exit While
                                                End If
                                            End While
                                            Players(playerNumber).Name = Encoding.UTF7.GetString(recv, (playerArrStartingIndex + 13), playerNameLength)
                                            JJ2ClientsSockInfo(joinedClientSocketIndex).NumOfPlayers += 1
                                            playerArrStartingIndex += 14 + playerNameLength

                                            RaiseEvent Player_Joined_Event(Players(playerNumber).Name, playerNumber, joinedClientSocketIndex, Players(playerNumber).Character, Players(playerNumber).Team, UserData)
                                        End If
                                    Next
                                    ActiveClients(joinedClientSocketIndex) = True
                                    RaiseEvent Client_Connected_Event(joinedClientSocketIndex, JJ2ClientsSockInfo(joinedClientSocketIndex).NumOfPlayers, UserData)
                                End If
                            Case &H12 'Player list update
                                Dim totNumOfPlayers As Byte = recv(packStartingIndex + 2)
                                Dim updatedClientsIndices As New List(Of Byte)
                                Dim updatedPlayersIDs As New List(Of Byte)
                                Dim numOfPlayersDoneWithInSock(_connectionlimit) As Byte
                                If totNumOfPlayers <> 0 Then
                                    Dim playerArrStartingIndex As Int16 = packStartingIndex + 3

                                    For workingOnPlayer As Byte = 0 To totNumOfPlayers - 1
                                        Dim playerSocketIndex As Byte = recv(playerArrStartingIndex)
                                        Dim playerNumber As Byte = recv(playerArrStartingIndex + 1)
                                        Dim charTeam As Byte = recv(playerArrStartingIndex + 2)

                                        If playerSocketIndex < JJ2ClientsSockInfo.Length And playerNumber < Players.Length Then
                                            updatedClientsIndices.Add(playerSocketIndex)
                                            updatedPlayersIDs.Add(playerNumber)

                                            'init socket info
                                            If JJ2ClientsSockInfo(playerSocketIndex) Is Nothing Then
                                                JJ2ClientsSockInfo(playerSocketIndex) = New JJ2SocketInfo
                                            Else
                                                '    JJ2ClientsSockInfo(playerSocketIndex).reset()
                                            End If
                                            JJ2ClientsSockInfo(playerSocketIndex).playerID(numOfPlayersDoneWithInSock(playerSocketIndex)) = playerNumber
                                            numOfPlayersDoneWithInSock(playerSocketIndex) += 1
                                            ActiveClients(playerSocketIndex) = True

                                            'init player
                                            If Players(playerNumber) Is Nothing Then
                                                Players(playerNumber) = New JJ2Player(playerSocketIndex, charTeam Mod &H10, recv(playerArrStartingIndex + 3), JJ2ClientsSockInfo(playerSocketIndex))
                                            Else
                                            '   Players(playerNumber).reset()
                                            Players(playerNumber).Update(playerSocketIndex, charTeam Mod &H10, recv(playerArrStartingIndex + 3), JJ2ClientsSockInfo(playerSocketIndex))
                                        End If
                                            Players(playerNumber).ClearStats(_plusServer)

                                        Array.Copy(recv, playerArrStartingIndex + 4, Players(playerNumber).Color, 0, 4)
                                        Dim playerNameLength As Byte = 0
                                            Dim whileHelper As UShort = playerArrStartingIndex + 14
                                            While recv(whileHelper) <> &H0
                                                playerNameLength += 1
                                                whileHelper += 1
                                                If recv.Length = whileHelper Then
                                                    Exit While
                                                End If
                                            End While
                                            Players(playerNumber).Name = Encoding.UTF7.GetString(recv, (playerArrStartingIndex + 14), playerNameLength)

                                            'assign NumOfPlayers
                                            Dim tempNumOfPlayersFromClient As Byte = 0
                                            For Each b As Byte In JJ2ClientsSockInfo(playerSocketIndex).playerID
                                                If b <> &HFF Then
                                                    tempNumOfPlayersFromClient += 1
                                                End If
                                            Next
                                            JJ2ClientsSockInfo(playerSocketIndex).NumOfPlayers = tempNumOfPlayersFromClient

                                            playerArrStartingIndex += 15 + playerNameLength
                                            'MsgBox(Players(playerNumber).Name & " in server")
                                        End If
                                    Next
                                End If
                                RaiseEvent Players_List_Update_Event(updatedPlayersIDs.ToArray, updatedClientsIndices.ToArray, UserData)
                            Case &H13
                                ChangeLevel(_nextLevelName, 0)
                                Dim UDPPacket9_2 As Byte() = {&H0, &H0, &H9, &HC0, CheckDatafrom10for9(0), CheckDatafrom10for9(1), CheckDatafrom10for9(2), CheckDatafrom10for9(3)}
                                Winsock2SendData(UDPPacket9_2)

                                udpTimerState = True
                                If _ID <> &HFF Then
                                    If _AutoSpec <> 0 Then
                                        Dim spectating As Byte() = {&H3, &H42, &H21}
                                        Winsock1SendData(spectating)
                                    End If
                                    RaiseEvent Joined_Event(socketID, _ID, _serverIPAddress, _serverAddress, _serverPort, UserData)

                                End If
                            Case &H16
                                If recv.Length - packStartingIndex > 11 Then
                                    Dim levelNameLength As Integer = recv(packStartingIndex + 10) - 1
                                    If recv.Length - packStartingIndex >= 11 + levelNameLength Then 'recv(10)=lvlNameLeng
                                        _nextLevelName = Encoding.ASCII.GetString(recv, packStartingIndex + 11, levelNameLength)
                                        Dim levelCRC32 = BitConverter.ToInt32(recv, (packStartingIndex + 11 + levelNameLength + 1))
                                        PlusCheck(0) = recv(packStartingIndex + 11 + levelNameLength + 1)
                                        PlusCheck(1) = recv(packStartingIndex + 11 + levelNameLength + 2)
                                        PlusCheck(2) = recv(packStartingIndex + 11 + levelNameLength + 3)
                                        PlusCheck(3) = recv(packStartingIndex + 11 + levelNameLength + 4)
                                        ChangeLevel(_nextLevelName, levelCRC32)
                                        udpTimerState = True
                                    End If
                                End If
                            Case &H17
                                'Battle: 0E 17 01 05 04 01 05 02 00 03 02 04 03 05 
                                'TB: 17 17 01 04 02 01 01 02 00 03 03 04 05 00 03 01 03 02 02 04 01 05 03 
                                ''''_gameInProgress = False
                                _cycling = True
                                Dim winnerID As Integer = &HFF 'Applicable for vanilla JJ2
                                Dim winnerScore As Integer = -1 'Applicable for vanilla JJ2

                                Dim currIndex As Integer = packStartingIndex + 3
                                Dim numOfTeams As Byte
                                Dim teamsIDs() As Byte
                                Dim teamsPlaces() As Byte

                                'Determine the number of teams in packet
                                If GameType = JJ2_Game_Type.CAPTURE Then
                                    numOfTeams = recv(currIndex)
                                    currIndex += 1 'point to the first array
                                Else 'teams are disabled
                                    numOfTeams = 0
                                End If

                                'Read teams places (in case there are teams)
                                ReDim teamsIDs(numOfTeams - 1)
                                ReDim teamsPlaces(numOfTeams - 1)
                                For i = 0 To numOfTeams - 1
                                    teamsIDs(i) = recv(currIndex)
                                    teamsPlaces(i) = recv(currIndex + 1)
                                    currIndex += 2
                                Next

                                'Read players places
                                Dim numOfPlayers = recv(currIndex)
                                Dim playersIDs(numOfPlayers - 1) As Byte
                                Dim playersPlaces(numOfPlayers - 1) As Byte
                                For i = 0 To numOfPlayers - 1
                                    playersIDs(i) = recv(currIndex)
                                    playersPlaces(i) = recv(currIndex + 1)
                                    currIndex += 2
                                Next
                                RaiseEvent End_Of_Level_Event(winnerID, winnerScore, playersIDs, playersPlaces, teamsIDs, teamsPlaces, UserData)
                            Case &H3F 'Plus Info
                                If packStartingIndex + 5 < recv.Length Then
                                    _serverPlusVersion = BitConverter.ToInt32(recv, packStartingIndex + 2)
                                End If
                                Dim boolsIndex As Integer = packStartingIndex + 9
                                If boolsIndex < recv.Length Then
                                    _plusOnly = CBool(recv(boolsIndex) And &H1)
                                    With PlusGameSettings
                                        .PlusOnly = _plusOnly
                                        .FriendlyFire = CBool(recv(boolsIndex) And &H2)
                                        .NoMovement = CBool(recv(boolsIndex) And &H4)
                                        .NoBliking = CBool(recv(boolsIndex) And &H8)
                                        .ReadyCommandEnabled = CBool(recv(boolsIndex) And &H32)
                                        .FireBall = CBool(recv(boolsIndex) And &H32)
                                    End With
                                End If
                                If boolsIndex + 1 < recv.Length Then '*New settings*
                                    PlusGameSettings.WallJumping = CBool(recv(boolsIndex + 1) And &H1)
                                End If

                            Case &H40 'Console Message
                                Dim consoleMessageType As Byte = recv(packStartingIndex + 2)
                                Dim bytear As Byte() = {&H0}
                                Dim str As String = Encoding.ASCII.GetString(bytear)
                                RaiseEvent Console_Message_Recveived_Event(Encoding.UTF7.GetString(recv).Substring(packStartingIndex + 3, packetRealLength - addnmbr - 3).Replace(vbNullChar, ""), consoleMessageType, UserData)
                            Case &H41
                                Dim packetType As Byte = recv(packStartingIndex + 2)
                                If packetType <> 0 Then 'Single Spectator
                                    Dim spectatorsSocketID As Byte = recv(packStartingIndex + 4)
                                    Dim value As Byte = &H1
                                    If recv(packStartingIndex + 5) = &HFE Then
                                        value = &H0
                                        '   SendMessage("/forcespectate " & JJ2ClientsSockInfo(spectatorsSocketID).playerID(0) + 1 & " on")
                                    End If
                                    '    If spectatorsSocketID < JJ2ClientsSockInfo.Length Then
                                    If spectatorsSocketID < JJ2ClientsSockInfo.Length Then 'this expression might ruin. makes a missing spectator
                                        If JJ2ClientsSockInfo(spectatorsSocketID) IsNot Nothing Then
                                        JJ2ClientsSockInfo(spectatorsSocketID).Spectating = CBool(value)
                                        'raise event
                                        Dim spectatorModeState As Boolean
                                            If value = 0 Then
                                                spectatorModeState = False
                                            Else
                                                spectatorModeState = True
                                            End If
                                            RaiseEvent Client_Spectate_Event(spectatorModeState, spectatorsSocketID, UserData)
                                            For Each anId As Byte In JJ2ClientsSockInfo(spectatorsSocketID).playerID
                                                If anId <> &HFF Then
                                                    RaiseEvent Player_Spectate_Event(spectatorModeState, anId, spectatorsSocketID, UserData)
                                                End If
                                            Next
                                        End If
                                    End If


                                    '  End If
                                Else 'Many Spectators represented by bits (1=ON)
                                    Dim numOfBitsToRead As Byte = recv(packStartingIndex + 3)

                                    If 8 * (recv.Length - (packStartingIndex + 4)) >= numOfBitsToRead Then 'Checks if these is no missing bits
                                        Dim numOfBytes As Byte = Math.Ceiling(numOfBitsToRead / 8)
                                        Dim BytesToBeConv(numOfBytes - 1) As Byte
                                        Array.Copy(recv, packStartingIndex + 4, BytesToBeConv, 0, numOfBytes)
                                        Dim SpectatingPlayers As New BitArray(BytesToBeConv)
                                        For i As Byte = 0 To numOfBitsToRead - 1
                                            If ActiveClients(i) = True And JJ2ClientsSockInfo(i) IsNot Nothing Then
                                            JJ2ClientsSockInfo(i).Spectating = SpectatingPlayers(i)
                                            'raise event
                                            Dim spectatorModeState As Boolean
                                                If SpectatingPlayers(i) = 0 Then
                                                    spectatorModeState = False
                                                Else
                                                    spectatorModeState = True
                                                End If
                                                RaiseEvent Client_Spectate_Event(spectatorModeState, i, UserData)
                                            Else
                                                '  Console.WriteLine("ERROR2! INVALID SPECTATOR ID,,, COULD NOT STORE " & i)
                                            End If
                                        Next
                                    End If
                                End If
                            Case &H43 'Remote admins
                                If False Then 'old code
                                    Dim intInBytes As Byte() = {recv(packStartingIndex + 4), recv(packStartingIndex + 5), recv(packStartingIndex + 6), recv(packStartingIndex + 7)}
                                    Dim loggedOnSocketID As Byte = GetAdminSocketID(intInBytes)
                                    If loggedOnSocketID = &HFE Then
                                        For i As Byte = 1 To 31
                                            If JJ2ClientsSockInfo(i) IsNot Nothing Then
                                            JJ2ClientsSockInfo(i).Admin = False
                                        End If
                                        Next
                                    JJ2ClientsSockInfo(0).Admin = 1
                                    '    Richtextbox1AppendText("*Remote admin has been disabled" & vbNewLine)
                                ElseIf loggedOnSocketID = &HFF Then
                                    Else
                                        If JJ2ClientsSockInfo(loggedOnSocketID) IsNot Nothing Then
                                        JJ2ClientsSockInfo(loggedOnSocketID).Admin = True
                                    End If
                                    End If
                                End If

                                If True Then 'new code
                                    Dim totalNumOfBits = recv(packStartingIndex + 3)
                                    For i = 0 To CInt(Math.Ceiling(totalNumOfBits / 32)) - 1  '(read byte by byte)
                                        Dim currentByte = recv(packStartingIndex + 4 + i)
                                        For i2 = 0 To 8 - 1 '(byte = 8bits)
                                            Dim clientIndex = i * 8 + i2
                                            If JJ2ClientsSockInfo(clientIndex) IsNot Nothing Then
                                            JJ2ClientsSockInfo(clientIndex).Admin = CBool(currentByte And (2 ^ i2))
                                        End If
                                        Next
                                    Next
                                End If
                                RaiseEvent Remote_Admins_Update_Event(UserData)
                            Case &H44 'Clients State
                            '13 44 00 20 00 0F 00 00 00 01 00 00 00 00 02 0F 00 00 00   <L=19
                            'LL 44 00 20 array{id,int32}

                            'There is an unknown byte comes before each collection of bits,
                            'mby it tells you what the bits are for,
                            '0=activeArray 1=downloadingArray 2=hasPlusArray
                            'confirmed
#If DEBUG Then
                            If recv(packStartingIndex + 2) <> 0 Then
                                    Console.WriteLine("recved packet 0x44 but the third byte is " & recv(packStartingIndex + 2))
                                End If
#End If

                            Dim startIndex = packStartingIndex + 4
                            While (startIndex + 4 < packStartingIndex + packetContentLength + 1)
                                Dim dataType = recv(startIndex) ' 0=activeArray 1=downloadingArray 2=hasPlusArray
                                Dim dataBytes As Byte() = {recv(startIndex + 1), recv(startIndex + 2), recv(startIndex + 3), recv(startIndex + 4)}
                                Dim actualData As New BitArray(dataBytes)
                                Select Case dataType
                                    Case 0 'active clients
                                        For i = 0 To 32 - 1
                                            If JJ2ClientsSockInfo(i) IsNot Nothing Then JJ2ClientsSockInfo(i).Active = actualData(i)
                                        Next
                                    Case 1 'downloading clients
                                        For i = 0 To 32 - 1
                                            If JJ2ClientsSockInfo(i) IsNot Nothing Then JJ2ClientsSockInfo(i).Downloading = actualData(i)
                                        Next
                                    Case 2 'client vanilla or plus
                                        For i = 0 To 32 - 1
                                            If JJ2ClientsSockInfo(i) IsNot Nothing Then JJ2ClientsSockInfo(i).Plus = actualData(i)
                                        Next
                                End Select
                                startIndex += 5
                            End While

                            If False Then
                                Dim clientActiveBytes As Byte() = {recv(packStartingIndex + 5), recv(packStartingIndex + 6), recv(packStartingIndex + 7), recv(packStartingIndex + 8)}
                                Dim isClientActive As New BitArray(clientActiveBytes)
                                Dim isClientDownloadingBytes As Byte() = {recv(packStartingIndex + 10), recv(packStartingIndex + 11), recv(packStartingIndex + 12), recv(packStartingIndex + 13)}
                                Dim isClientDownloading As New BitArray(isClientDownloadingBytes)
                                Dim clientHasJJ2PlusBytes As Byte() = {recv(packStartingIndex + 15), recv(packStartingIndex + 16), recv(packStartingIndex + 17), recv(packStartingIndex + 18)}
                                Dim clientHasJJ2Plus As New BitArray(clientHasJJ2PlusBytes)
                                For i = 0 To 32 - 1
                                    If JJ2ClientsSockInfo(i) IsNot Nothing Then
                                        JJ2ClientsSockInfo(i).Plus = clientHasJJ2Plus(i)
                                        JJ2ClientsSockInfo(i).Downloading = isClientDownloading(i)
                                        JJ2ClientsSockInfo(i).Active = isClientActive(i)
                                    End If
                                Next
                            End If

                            RaiseEvent Clients_State_Update_Event(UserData)
                            Case &H45
                                Dim newOrEndedGame As Boolean = False
                                timerInfoUpdateDate = Date.Now
                                Dim gameWasStarted = _gameStarted
                                _gameStarted = CBool(recv(packStartingIndex + 2) And &H1)
                                _gameState = recv(packStartingIndex + 2) >> 1
                                timeRemaining = BitConverter.ToInt32(recv, packStartingIndex + 3)
                                _timeLimit = BitConverter.ToInt32(recv, packStartingIndex + 7)
                                If _gameState <> JJ2Plus_Game_State.NORMAL Then
                                    If _gameStarted = False Then
                                        If TimeRemaining = _timeLimit Then
                                            If _gameInProgress Then
                                                'Current game Ended
                                                If gameWasStarted = False Then
                                                    'ACCURATE Current game Ended
                                                    '(gg)
                                                End If
                                                _gameInProgress = False
                                                newOrEndedGame = True
                                            Else
                                                'first time??
                                                newOrEndedGame = False
                                            End If

                                        Else
                                            'Game paused
                                            _gameInProgress = True
                                            newOrEndedGame = False
                                        End If
                                    Else
                                        If _gameInProgress Then
                                            'Game resumed
                                            newOrEndedGame = False
                                        Else
                                            'New game started
                                            newOrEndedGame = True
                                            _gameInProgress = True
                                            If _isFirstGameState Then

                                            End If

                                        End If
                                    End If
                                Else 'if timelimit is disabled
                                    _gameInProgress = _gameState
                                    newOrEndedGame = False
                                End If

                                RaiseEvent Game_State_Changed_Event(_gameStarted, gameWasStarted, _gameState, TimeRemaining, _timeLimit, newOrEndedGame, _isFirstGameState, UserData)
                                _isFirstGameState = False
                            Case &H47
                                '06 47 00 00 00 00 

                            Case &H49 'Pings
                                Dim numOfPingedPlayers As Byte = ((packetRealLength - addnmbr - 2) / 3)
                                Dim workingOnPlayer As Byte = &H1
                                Dim i As Integer = packStartingIndex + 2
                                While workingOnPlayer <= numOfPingedPlayers
                                    Dim playerID As Byte = recv(i)
                                    i += 1
                                    Dim pingInBytes As Byte() = {0, 0}
                                    pingInBytes(0) = recv(i)
                                    i += 1
                                    pingInBytes(1) = recv(i)
                                    i += 1
                                    Dim Ping As Integer = BitConverter.ToInt16(pingInBytes, 0)
                                    If Players(playerID) IsNot Nothing Then
                                        Players(playerID).latency = Ping
                                    End If
                                    workingOnPlayer += 1
                                End While
                                RaiseEvent Latency_Update_Event(UserData)

                            Case &H4A 'Game Settings
                                '09 4A 05 0B 14 00 00 00 0B 
                                '09 4A gameMode customGameMode maxScore[4] enabledTeamsBitwise
                                _gameType = recv(packStartingIndex + 2)
                                _customGameMode = recv(packStartingIndex + 3)
                                _maxScore = BitConverter.ToInt32(recv, packStartingIndex + 4)
                                Dim teamsState As Byte = recv(packStartingIndex + 8)
                                For teamID = 0 To 4 - 1
                                    Dim teamEnabled As Boolean = CBool((teamsState >> teamID) And &H1)
                                    If Teams(teamID).Enabled <> teamEnabled Then 'If changed
                                        Teams(teamID).Enabled = teamEnabled
                                        RaiseEvent Team_State_Change_Event(teamID, teamEnabled, UserData)
                                    End If
                                Next
                                RaiseEvent Game_Settings_Update_Event(_gameType, _customGameMode, _maxScore, UserData)
                                Console.WriteLine(_gameType & " " & _customGameMode & " " & _maxScore)
                            Case &H4B 'Team score info update
                                'subpacket: 00 4B unknown1 unknown2 numOfTeamScoreInfo array(numOfTeamScoreInfo){teamID, Score[4]}
                                Dim numOfTeamScoreInfo = recv(packStartingIndex + 4)
                                Dim teamsUpdated As New List(Of Byte) 'for the event
                                Dim arrayStartIndex As Integer = packStartingIndex + 5
                                For i = 0 To numOfTeamScoreInfo - 1
                                    'array{teamID, Score[4]}
                                    Dim teamID = recv(arrayStartIndex)
                                    Dim oldScore As Integer = Teams(teamID).Score
                                    Teams(teamID).Score = BitConverter.ToInt32(recv, arrayStartIndex + 1)
                                    teamsUpdated.Add(teamID)
                                    RaiseEvent Gameplay_Team_Score_Set_Event(teamID, oldScore, Teams(teamID).Score, UserData)
                                    arrayStartIndex += 5
                                    Console.WriteLine("[TCP] score team" & teamID & "=" & Teams(teamID).Score)
                                Next
                                RaiseEvent Gameplay_Teams_Scores_Set_Event(teamsUpdated.ToArray, UserData)
                            Case &H4C 'Player stats list
                                '00 4C array{playerID, Roasts[4], Deaths[4]} 
                                Dim numOfPlayers As Integer = (packetContentLength - 1) / 9 '(packetContentLength - 1) is data length without JJ2 TCP packet header, 9 is array length
                                Dim arrayStartIndex = packStartingIndex + 2
                                For i = 1 To numOfPlayers
                                    If arrayStartIndex + 8 >= recv.Length Then 'if packet is not long enough/invalid
                                        Exit For
                                    End If
                                    Dim playerID As Byte = recv(arrayStartIndex)
                                    Players(playerID).Roasts = BitConverter.ToInt32(recv, arrayStartIndex + 1)
                                    Players(playerID).Deaths = BitConverter.ToInt32(recv, arrayStartIndex + 5)
                                    arrayStartIndex += 9 '9 is array length
                                Next
                                RaiseEvent Gameplay_Player_Stats_List_Update_Event(UserData)
                            Case &H4D 'Player deaths update
                                Dim playerID = recv(packStartingIndex + 2)
                                If playerID < Players.Length AndAlso Players(playerID) IsNot Nothing Then 'Check if not null
                                    Players(playerID).Deaths = BitConverter.ToInt32(recv, packStartingIndex + 3)
                                    RaiseEvent Gameplay_Player_Deaths_Update_Event(playerID, Players(playerID).Deaths, UserData)
                                    Console.WriteLine("player [" & Players(playerID).Name & "] D=" & Players(playerID).Deaths)
                                Else
                                    RaiseEvent Gameplay_Player_Deaths_Update_Event(playerID, 0, UserData)
                                    Console.WriteLine("player[" & playerID & "] D=" & BitConverter.ToInt32(recv, packStartingIndex + 3))
                                End If
                            Case &H51
#If DEBUG Then
                                Console.WriteLine("Received packet 0x51")
#End If
                            Case &H52 'Idle server mode
                                _idleServerMode = CBool(recv(packStartingIndex + 2))
                            If JJ2ClientsSockInfo(0) IsNot Nothing Then JJ2ClientsSockInfo(0).Idle = _idleServerMode
                            RaiseEvent Idle_Server_Mode_Update_Event(_idleServerMode, UserData)
                            Case &H55 'Set max resolution
                                If packStartingIndex + 5 < recv.Length Then
                                    Dim w As UShort = BitConverter.ToUInt16(recv, packStartingIndex + 2)
                                    Dim h As UShort = BitConverter.ToUInt16(recv, packStartingIndex + 4)
                                    PlusGameSettings.MaxResolutionWidth = w
                                    PlusGameSettings.MaxResolutionHeight = h
                                    RaiseEvent Max_Resolution_Set_Event(w, h, UserData)
                                End If
                            Case &H56 'Script packet
                                Dim plusNetworkStreamSourceId As Byte = recv(packStartingIndex + 2) 'mut id (not sure)
                                Dim fullRecvPacket(packetRealLength - 1) As Byte
                                Dim plusNetworkStreamLength As Integer = fullRecvPacket.Length - (3 - addnmbr)
                                Dim plusNetworkStreamData(plusNetworkStreamLength) As Byte
                                Array.Copy(recv, Starting, fullRecvPacket, 0, fullRecvPacket.Length)
                                Array.Copy(fullRecvPacket, 3 + addnmbr, plusNetworkStreamData, 0, plusNetworkStreamLength)
                                RaiseEvent JJ2_Plus_Network_Stream_Data_Arrival(plusNetworkStreamData, plusNetworkStreamSourceId)
                            Case &H58 'Whisper
                                If (recv(Starting + 2) < &H64) Then ' recv(Starting + 2) is player ID
                                    ''Received whisper from player
                                    Dim sourcePlayerID As Byte = recv(Starting + 2)
                                    Dim message As String = DefaultEncoding.GetString(recv, Starting + 3, packetRealLength - 3)

                                Else
                                    ''Your whisper to player arrived
                                    Dim recvPlayerID As Byte = recv(Starting + 2) - &H64
                                End If

                            Case &H5A ' mut list
                                scriptModules.Clear()
                                scriptsRequiredFiles.Clear()
                                If recv.Length - packStartingIndex >= 11 Then
                                    Dim numOfScripts As Byte = recv(packStartingIndex + 7)
                                    Dim numOfRequiredFiles As UShort = BitConverter.ToInt16(recv, packStartingIndex + 8)
                                    Dim fileStartIndex As Integer = packStartingIndex + 11
                                    Dim fileName As String
                                    Dim scriptModuleId As Byte = 0
                                    Dim temp As Integer
                                    For i As Integer = 0 To numOfScripts + numOfRequiredFiles - 1
                                        Dim fileCRC32 As Int32 = BitConverter.ToInt32(recv, fileStartIndex + 1)
                                        temp = fileStartIndex + 5 'file name length byte index.
                                        If recv(temp) + temp < recv.Length Then
                                            fileName = DefaultEncoding.GetString(recv, fileStartIndex + 6, recv(temp)).ToLower
                                            If recv(fileStartIndex) = 0 Then 'script
                                                If fileName.EndsWith(".mut") Then
                                                    scriptModuleId += 1
                                                End If
                                                scriptModules.Add(fileName, scriptModuleId)
                                            Else 'requiredFile
                                                scriptsRequiredFiles.Add(fileName)
                                            End If
                                        Else
                                            Console.WriteLine("missing files 0 to " & (numOfScripts + numOfRequiredFiles) & " - " & numOfScripts & " " & numOfRequiredFiles & " " & recv(packStartingIndex + 8))
                                            Exit For
                                        End If
                                        fileStartIndex += recv(temp) + 6
                                    Next
                                End If

                        End Select



                        '''''''''''''
                        ''Ending the while loop:
                        If (Starting + packetRealLength) >= recv.Length Then
                            readingComplete = True
                        Else
                            Starting = Starting + packetRealLength
                        End If
                        packetLength = 0
                        packetRealLength = 0
                        addnmbr = 0
                        packetNum += 1

                        ' End If ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
                    End While

                End If
            ''''''''Catch ex As Exception
            ''''''''RaiseEvent Error_Event(False, 2, ex.Message, UserData)
            ''''''''Finally
            ''''''''End Try
        End Sub
        Private Sub Winsock1_DataArrival_Read(ByVal recv As Byte())
            Try
                If recv.Length >= 2 Then
                    Dim packetLength As Integer = 0
                    Dim packetRealLength As Integer = 0
                    Dim Starting As Integer = 0
                    Dim addnmbr As Integer = 0
                    Dim packetID As Byte = &H0
                    Dim packetNum As Integer = 0
                    Dim readingComplete As Boolean = False
                    Dim packStartingIndex As Integer = 0

                    While readingComplete = False
                        '    If recv.Length > Starting + addnmbr + 1 Then '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
                        If Not (Starting + 0) < recv.Length Then 'new thing prevents OutOfRange reading
                            Exit While
                        End If
                        If recv(Starting + 0) = 0 Then
                            If Starting + 1 = recv.Length Then
                                Exit While
                            ElseIf Starting + 1 < recv.Length Then
                                If recv(Starting + 1) = 0 Then
                                    Exit While
                                End If
                            End If
                            Dim packetLengthInBytes As Byte() = {recv(Starting + 1), recv(Starting + 2)}
                            packetLength = BitConverter.ToInt16(packetLengthInBytes, 0)
                            packetRealLength = BitConverter.ToInt16(packetLengthInBytes, 0) + 3
                            addnmbr = 2
                        End If
                        If recv(Starting + 0) <> 0 Then
                            If recv(Starting) > (recv.Length - Starting) Then
                                Exit While
                            End If
                            packetLength = recv(Starting + 0)
                            packetRealLength = recv(Starting + 0)
                            addnmbr = 0
                        End If
                        packStartingIndex = Starting + addnmbr
                        packetID = recv(packStartingIndex + 1)
                        '''''''''''''

                        Select Case packetID
                            Case &H1B 'Chat message
                                Dim socketIDandTeam As Byte = recv(Starting + addnmbr + 2)
                                Dim senderSocketID As Byte = socketIDandTeam And 15
                                Dim team As Byte = Math.Floor(socketIDandTeam / 16)
                                Dim message As String = System.Text.Encoding.Default.GetString(recv, Starting + addnmbr + 4, packetLength - 4)
                                Dim senderPlayerID As Byte = JJ2ClientsSockInfo(senderSocketID).playerID(0)

                                If senderPlayerID <> &HFF Then
                                    RaiseEvent Message_Received_Event(message, Players(senderPlayerID).Name, team, senderPlayerID, senderSocketID, UserData)
                                End If
                            Case &H12
                                Dim totNumOfPlayers As Byte = recv(Starting + addnmbr + 2)
                                Dim numOfPlayersDoneWithInSock(_connectionlimit) As Byte
                                If totNumOfPlayers <> 0 Then
                                    Dim playerArrStartingIndex As Int16 = Starting + addnmbr + 3

                                    For workingOnPlayer As Byte = 0 To totNumOfPlayers - 1
                                        Dim playerSocketIndex As Byte = recv(playerArrStartingIndex)
                                        Dim playerNumber As Byte = recv(playerArrStartingIndex + 1)
                                        Dim charTeam As Byte = recv(playerArrStartingIndex + 2)

                                        If playerSocketIndex < JJ2ClientsSockInfo.Length And playerNumber < Players.Length Then
                                            'init socket info
                                            If JJ2ClientsSockInfo(playerSocketIndex) Is Nothing Then
                                                JJ2ClientsSockInfo(playerSocketIndex) = New JJ2SocketInfo
                                            Else
                                                '    JJ2ClientsSockInfo(playerSocketIndex).reset()
                                            End If

                                            JJ2ClientsSockInfo(playerSocketIndex).playerID(numOfPlayersDoneWithInSock(playerSocketIndex)) = playerNumber
                                            numOfPlayersDoneWithInSock(playerSocketIndex) += 1
                                            ActiveClients(playerSocketIndex) = True

                                            'init player
                                            If Players(playerNumber) Is Nothing Then
                                                Players(playerNumber) = New JJ2Player(playerSocketIndex, charTeam Mod &H10, Math.Floor(charTeam / &H10), JJ2ClientsSockInfo(playerSocketIndex))
                                            Else
                                                '   Players(playerNumber).reset()
                                                Players(playerNumber).Update(playerSocketIndex, charTeam Mod &H10, Math.Floor(charTeam / &H10), JJ2ClientsSockInfo(playerSocketIndex))
                                            End If

                                            Array.Copy(recv, playerArrStartingIndex + 3, Players(playerNumber).Color, 0, 4)
                                            Dim playerNameLength As Byte = 0
                                            Dim whileHelper As Byte = playerArrStartingIndex + 7
                                            While recv(whileHelper) <> &H0
                                                playerNameLength += 1
                                                whileHelper += 1
                                                If recv.Length = whileHelper Then
                                                    Exit While
                                                End If
                                            End While
                                            Players(playerNumber).Name = Encoding.UTF7.GetString(recv, (playerArrStartingIndex + 7), playerNameLength)

                                            'assign NumOfPlayers
                                            Dim tempNumOfPlayersFromClient As Byte = 0
                                            For Each b As Byte In JJ2ClientsSockInfo(playerSocketIndex).playerID
                                                If b <> &HFF Then
                                                    tempNumOfPlayersFromClient += 1
                                                End If
                                            Next
                                            JJ2ClientsSockInfo(playerSocketIndex).NumOfPlayers = tempNumOfPlayersFromClient

                                            playerArrStartingIndex += 8 + playerNameLength
                                            'MsgBox(Players(playerNumber).Name & " in server")
                                        End If
                                    Next
                                End If
                            Case &H11
                                Dim joinedClientSocketIndex As Byte = recv(Starting + addnmbr + 2)
                                Dim numIfJoinedPlayers As Byte = recv(Starting + addnmbr + 3)
                                Dim playerArrStartingIndex As UInt16 = Starting + addnmbr + 4
                                If joinedClientSocketIndex < _connectionlimit Then
                                    'init socket info
                                    If JJ2ClientsSockInfo(joinedClientSocketIndex) Is Nothing Then
                                        JJ2ClientsSockInfo(joinedClientSocketIndex) = New JJ2SocketInfo
                                    Else
                                        JJ2ClientsSockInfo(joinedClientSocketIndex).reset()
                                    End If

                                    For workingOnPlayer As Byte = 0 To numIfJoinedPlayers - 1
                                        Dim playerNumber As Byte = recv(playerArrStartingIndex)
                                        Dim charTeam As Byte = recv(playerArrStartingIndex + 1)



                                        If joinedClientSocketIndex < JJ2ClientsSockInfo.Length And playerNumber < Players.Length Then
                                            JJ2ClientsSockInfo(joinedClientSocketIndex).playerID(workingOnPlayer) = playerNumber
                                            Dim fdgfdgdfgdfgfdg As Byte = JJ2ClientsSockInfo(joinedClientSocketIndex).playerID(workingOnPlayer)

                                            'init player
                                            If Players(playerNumber) Is Nothing Then
                                                Players(playerNumber) = New JJ2Player(joinedClientSocketIndex, charTeam Mod &H10, Math.Floor(charTeam / &H10), JJ2ClientsSockInfo(joinedClientSocketIndex))
                                            Else
                                                Players(playerNumber).reset()
                                                Players(playerNumber) = New JJ2Player(joinedClientSocketIndex, charTeam Mod &H10, Math.Floor(charTeam / &H10), JJ2ClientsSockInfo(joinedClientSocketIndex))
                                            End If
                                            Array.Copy(recv, playerArrStartingIndex + 2, Players(playerNumber).Color, 0, 4)

                                            Dim playerNameLength As Byte = 0
                                            Dim whileHelper As Byte = playerArrStartingIndex + 6
                                            While recv(whileHelper) <> &H0
                                                playerNameLength += 1
                                                whileHelper += 1
                                                If recv.Length = whileHelper Then
                                                    Exit While
                                                End If
                                            End While
                                            Players(playerNumber).Name = Encoding.UTF7.GetString(recv, (playerArrStartingIndex + 6), playerNameLength)
                                            JJ2ClientsSockInfo(joinedClientSocketIndex).NumOfPlayers += 1
                                            playerArrStartingIndex += 7 + playerNameLength

                                            RaiseEvent Player_Joined_Event(Players(playerNumber).Name, playerNumber, joinedClientSocketIndex, Players(playerNumber).Character, Players(playerNumber).Team, UserData)
                                        End If

                                    Next
                                    ActiveClients(joinedClientSocketIndex) = True
                                    RaiseEvent Client_Connected_Event(joinedClientSocketIndex, JJ2ClientsSockInfo(joinedClientSocketIndex).NumOfPlayers, UserData)
                                End If
                            Case &HD

                                Dim disconnectMsg As Byte = recv(Starting + addnmbr + 2)
                                Dim leftClientSocketIndex As Byte = recv(Starting + addnmbr + 3)
                                If leftClientSocketIndex <> socketID Then
                                    If leftClientSocketIndex < JJ2ClientsSockInfo.Length Then
                                        ActiveClients(leftClientSocketIndex) = False
                                        If JJ2ClientsSockInfo(leftClientSocketIndex) IsNot Nothing Then
                                            If CBool(JJ2ClientsSockInfo(leftClientSocketIndex).NumOfPlayers) Then
                                                RaiseEvent Client_Disconnected_Event(leftClientSocketIndex, disconnectMsg, JJ2ClientsSockInfo(leftClientSocketIndex).NumOfPlayers, UserData)
                                                For wokingOnPlayer As Byte = 0 To JJ2ClientsSockInfo(leftClientSocketIndex).NumOfPlayers - 1
                                                    If JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) <> &HFF And JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) < (Players.Length + 1) Then
                                                        Dim playerName As String = Players(JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)).Name
                                                        Dim playerID As Byte = JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)
                                                        If Players(JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)) IsNot Nothing Then
                                                            Players(JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer)).reset()
                                                        End If
                                                        JJ2ClientsSockInfo(leftClientSocketIndex).playerID(wokingOnPlayer) = &HFF
                                                        RaiseEvent Player_Left_Event(playerName, disconnectMsg, playerID, leftClientSocketIndex, UserData)
                                                    End If
                                                Next
                                                JJ2ClientsSockInfo(leftClientSocketIndex).reset()
                                            End If
                                        End If
                                    End If
                                Else
                                    'You
                                    WinsockClose(disconnectMsg)
                                End If

                            Case &H10
                                socketID = recv(Starting + addnmbr + 2)
                                Dim levelNameLength As Byte = recv(Starting + addnmbr + 4)
                                socketID = recv(Starting + addnmbr + 2)
                                _ID = recv(Starting + addnmbr + 3)
                                _nextLevelName = Encoding.ASCII.GetString(recv).Substring(packStartingIndex + 5, levelNameLength)
                                Dim joiningData2Part1 As Byte() = {0, &HE, &H1, &H1, &H1, &H18, &H20, &H28, &H11} '{&H19, &HE, &H1, &H1, &H0, &H1, &H10, &H18, &H20, &H28, &H11, &H1, &HA, &HD, &H0, &H0, &H52, &H65, &H63, &H6F, &H72, &H64, &H65, &H72, &H0}
                                Dim joiningData2Part2 As Byte() = DefaultNameEncoding.GetBytes(_name)
                                Dim joiningData2(joiningData2Part1.Length + joiningData2Part2.Length) As Byte
                                Array.Copy(joiningData2Part1, joiningData2, joiningData2Part1.Length)
                                Array.Copy(joiningData2Part2, 0, joiningData2, joiningData2Part1.Length, joiningData2Part2.Length)
                                joiningData2(0) = joiningData2.Length
                                If Not _packet0x0EWasSent Then
                                    _packet0x0EWasSent = True
                                    Winsock1SendData(joiningData2)
                                End If
                            Case &H16
                                If recv.Length - Starting - addnmbr > 11 Then
                                    If (recv.Length - Starting - addnmbr) >= (11 + recv(10 + Starting)) Then 'recv(10)=lvlNamwLeng
                                        _nextLevelName = Encoding.ASCII.GetString(recv, Starting + addnmbr + 11, recv(10 + Starting))
                                        ChangeLevel(_nextLevelName, 0)
                                    End If
                                End If
                            Case &H17
                                _cycling = True
                                Dim winnerID As Integer = recv(packStartingIndex + 2)
                                Dim winnerScore As Integer = recv(packStartingIndex + 7)
                                Dim teamsIDs() As Byte = {} 'for JJ2+ only
                                Dim teamsPlaces() As Byte = {} 'for JJ2+ only
                                Dim playersIDs As Byte() = {} 'for JJ2+ only
                                Dim playersPlaces As Byte() = {} 'for JJ2+ only
                                If _gameType = JJ2_Game_Type.CAPTURE Then
                                    teamsIDs = {winnerID}
                                    teamsPlaces = {1}
                                Else
                                    playersIDs = {winnerID}
                                    playersPlaces = {1}
                                End If
                                RaiseEvent End_Of_Level_Event(winnerID, winnerScore, playersIDs, playersPlaces, teamsIDs, teamsPlaces, UserData)
                            Case &H13
                                ChangeLevel(_nextLevelName, 0)
                            Case 151
                        End Select



                        '''''''''''''
                        ''Ending the while loop:
                        If (Starting + packetRealLength) >= recv.Length Then
                            readingComplete = True
                        Else
                            Starting = Starting + packetRealLength
                        End If
                        packetLength = 0
                        packetRealLength = 0
                        addnmbr = 0
                        packetNum += 1

                        ' End If ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
                    End While

                End If
            Catch ex As Exception
                RaiseEvent Error_Event(False, 1, ex.Message, UserData)
            End Try
        End Sub
        Private Sub Winsock2GoReceive()
            Try
                Winsock2.Client.BeginReceiveFrom(BufferUDP, 0, BufferUDP.Length, SocketFlags.None, _remoteEP, New AsyncCallback(AddressOf Winsock2_DataArrival), _remoteEP)
            Catch sockEx As SocketException
                WinsockClose(7)
            Catch nullEx As NullReferenceException
            Catch obDisEx As ObjectDisposedException
            End Try
        End Sub
        Dim BufferUDP(512 - 1) As Byte
        Private Sub Winsock2_DataArrival(ByVal ar As IAsyncResult)
            Try
                Dim dataLength As Integer = Winsock2.Client.EndReceiveFrom(ar, ar.AsyncState)
                TotalBytesRecv += dataLength
                ConnectionTimeOut = 30
                Dim recv(dataLength - 1) As Byte
                Array.Copy(BufferUDP, recv, dataLength)
                If _plusServer = False Then
                    Winsock2_DataArrival_Read(recv)
                Else
                    Winsock2_DataArrival_Read_Plus(recv)
                End If
                Winsock2GoReceive()
            Catch sockEx As SocketException
                WinsockClose(7)
                '   MsgBox("Winsock2_DataArrival err")
            Catch nullEx As NullReferenceException
            Catch obDisEx As ObjectDisposedException

            End Try
        End Sub
        Private Sub Winsock2_DataArrival_Read(ByVal recv As Byte())
            Try
                If _connected AndAlso recv.Length > 2 Then
                    Dim packetID As Byte = recv(2)

                    Select Case packetID
                        Case &H7
                            Dim subpacketStartIndex = 4
                            Dim subpacketLength As Integer
                            Dim subpacketID As Byte
                            While (subpacketStartIndex < recv.Length)
                                subpacketID = recv(subpacketStartIndex)
                                subpacketLength = GetGameplaySubpacketLength(subpacketID)
                                If subpacketLength <= 0 Then
                                    Exit While
                                End If
                                Select Case subpacketID
                                    Case &H7 'Bullet fired

                                    Case &HA

                                    Case &HC 'Hurt
                                    Case &HE 'Kill
                                        Dim victimID As Byte = recv(subpacketStartIndex + 1)
                                        Dim victimKills As Integer = recv(subpacketStartIndex + 2)
                                        Dim killerID As Byte = recv(subpacketStartIndex + 3)
                                        Dim killerKills As Integer = recv(subpacketStartIndex + 4)
                                        If victimID < Players.Length Then
                                            Players(victimID).Roasts = (victimKills)
                                        End If
                                        If killerID < Players.Length Then
                                            Players(killerID).Roasts = (killerKills)
                                        End If
                                        RaiseEvent Gameplay_Player_Roast_Event(victimID, victimKills, killerID, killerKills, UserData)
                                    Case &HF 'Hit by special move
                                        Dim victimID As Byte = recv(subpacketStartIndex + 1) And CByte(32 - 1)
                                        Dim victimHealth As Byte = recv(subpacketStartIndex + 1) And CByte(&HE0)
                                        Dim attackerID As Byte = recv(subpacketStartIndex + 2) And CByte(32 - 1)
                                        RaiseEvent Gameplay_Player_Hit_Event(victimID, victimHealth, attackerID, UserData)
                                    Case &H1E 'CTF info
                                        Dim redCarier As Byte
                                        Dim blueCarier As Byte
                                End Select
                                subpacketStartIndex += 1 + subpacketLength
                            End While
                        Case &H9
                            If recv.Length > 8 Then
                                Dim Packet9SendBack As Byte() = {0, 0, &H9, UDPCount, 0, recv(5), recv(6), recv(7), recv(8)}
                                UDPchecksum(Packet9SendBack)
                                If ExtraLatency > 0 Then
                                    System.Threading.Thread.Sleep(ExtraLatency)
                                End If
                                Winsock2SendData(Packet9SendBack)
                            End If
                    End Select
                End If
            Catch ex As Exception
                RaiseEvent Error_Event(False, 3, ex.Message, UserData)
            Finally
            End Try
        End Sub
        Private Sub Winsock2_DataArrival_Read_Plus(ByVal recv As Byte())
            Try
                If _connected AndAlso recv.Length > 2 Then
                    Dim packetID As Byte = recv(2)

                    Select Case packetID
                        Case &H7
                            Dim subpacketStartIndex = 4
                            Dim subpacketLength As Integer
                            Dim subpacketID As Byte
                            While (subpacketStartIndex < recv.Length)
                                subpacketID = recv(subpacketStartIndex)
                                subpacketLength = GetPlusGameplaySubpacketLength(subpacketID)
                                If subpacketLength <= 0 Then
#If DEBUG Then
                                    Console.WriteLine("Length of subpacket [" & subpacketID & "] is unknown")
#End If
                                    Exit While
                                End If
                                Select Case subpacketID
                                    Case &H3
                                        subpacketLength = 3 'for plus?
                                    Case &H7 'Bullet fired

                                    Case &HA
                                        'subpacketLength = 3 'for plus?
                                    Case &HC 'Hurt
                                    Case &HE 'Kill
                                        Dim victimID As Byte = recv(subpacketStartIndex + 1)
                                        Dim victimKills As Integer = BitConverter.ToInt32(recv, subpacketStartIndex + 2)
                                        Dim killerID As Byte = recv(subpacketStartIndex + 6)
                                        Dim killerKills As Integer = BitConverter.ToInt32(recv, subpacketStartIndex + 7)
                                        If victimID < Players.Length Then
                                            Players(victimID).Roasts = (victimKills)
                                        End If
                                        If killerID < Players.Length Then
                                            Players(killerID).Roasts = (killerKills)
                                        End If
                                        RaiseEvent Gameplay_Player_Roast_Event(victimID, victimKills, killerID, killerKills, UserData)
                                    Case &HF 'Hit by special move
                                        Dim victimID As Byte = recv(subpacketStartIndex + 1) And CByte(32 - 1)
                                        Dim victimHealth As Byte = (recv(subpacketStartIndex + 1) And CByte(&HE0)) >> 5
                                        Dim attackerID As Byte = recv(subpacketStartIndex + 2) And CByte(32 - 1)
                                        RaiseEvent Gameplay_Player_Hit_Event(victimID, victimHealth, attackerID, UserData)
                                    Case &H10 'Bullet(Plus)??
                                        subpacketLength = 20 'for plus?
                                        Dim gun As Byte = recv(subpacketStartIndex + 7) And &HF
                                        Dim isGunPU As Boolean = CBool(recv(subpacketStartIndex + 7) And &H80)
                                        Dim vertical As Boolean = CBool(recv(subpacketStartIndex + 7) And &H40) 'not sure????????

                                        Dim bulletX As Short = BitConverter.ToInt16(recv, subpacketStartIndex + 8)
                                        Dim bulletY As Short = BitConverter.ToInt16(recv, subpacketStartIndex + 10)

                                        'bullet velocity is not final, playVx will affect it
                                        Dim bulletVx As SByte = If(recv(subpacketStartIndex + 13) < 128, recv(subpacketStartIndex + 13), recv(subpacketStartIndex + 13) - 256)
                                        Dim bulletAngle As SByte = If(recv(subpacketStartIndex + 14) < 128, recv(subpacketStartIndex + 14), recv(subpacketStartIndex + 14) - 256) 'a guess
                                        Dim bulletVy As SByte = If(recv(subpacketStartIndex + 15) < 128, recv(subpacketStartIndex + 15), recv(subpacketStartIndex + 15) - 256)

                                        Dim playVx As SByte = If(recv(subpacketStartIndex + 16) < 128, recv(subpacketStartIndex + 16), recv(subpacketStartIndex + 16) - 256) 'i think
                                        Dim direction As SByte = If(recv(subpacketStartIndex + 17) < 128, recv(subpacketStartIndex + 17), recv(subpacketStartIndex + 17) - 256)

                                        Dim actualAngle As Single 'clockwise, starting form right (like some 2d game frameworks)


                                        Dim lifetime As Byte = recv(subpacketStartIndex + 18)
                                        Dim ammo As Byte = recv(subpacketStartIndex + 20)

                                        RaiseEvent Gameplay_Plus_Bullet_Shoot_Event(gun, isGunPU, bulletX, bulletY, bulletVx, bulletVy, bulletAngle, playVx, lifeTime, ammo, direction, vertical, &HFF, False, Me.UserData)

#If DEBUG Then
                                     '   Console.WriteLine("Aim val [" & recv(subpacketStartIndex + 14) & "]")
#End If
                                    Case &H1E 'CTF info
                                        'subpacket: 1E 00 02 numOfTeamInfo array(numOfTeamInfo){teamID, Score[4]} unknownByte numOfFlagInfo array(numOfFlagInfo){teamID, isFlagCaptured, carrierID}
                                        Dim numOfTeamScoreInfo = recv(subpacketStartIndex + 3)
                                        Dim teamsUpdated(numOfTeamScoreInfo - 1) As Byte 'for the event
                                        Dim arrayStartIndex As Integer = subpacketStartIndex + 4
                                        For i = 0 To numOfTeamScoreInfo - 1
                                            'array{teamID, Score[4]}
                                            If arrayStartIndex + 4 >= recv.Length Then '(arrayStartIndex + 4) is the index of the last byte from recv this loop will access
                                                Exit For 'prevents error
                                            End If
                                            Dim teamID = recv(arrayStartIndex)
                                            teamsUpdated(i) = teamID 'for event
                                            Dim oldScore As Integer = Teams(teamID).Score

                                            Teams(teamID).Score = BitConverter.ToInt32(recv, arrayStartIndex + 1)
                                            If Teams(teamID).Score <> oldScore Then
                                                RaiseEvent Gameplay_Team_Scored_Event(teamID, oldScore, Teams(teamID).Score, UserData)
                                            End If
                                            RaiseEvent Gameplay_Team_Score_Update_Event(teamID, oldScore, Teams(teamID).Score, UserData)
                                            arrayStartIndex += 5
                                            Console.WriteLine("score team" & teamID & "=" & Teams(teamID).Score)
                                        Next
                                        RaiseEvent Gameplay_Teams_Scores_Update_Event(teamsUpdated, UserData)

                                        Dim unknownByte = recv(arrayStartIndex)
                                        Console.WriteLine("unknwnByte=" & unknownByte)
                                        If arrayStartIndex + 1 < recv.Length Then 'prevents error
                                            Dim numOfFlagInfo = recv(arrayStartIndex + 1)
                                            Dim teamFlagsUpdated(numOfFlagInfo - 1) As Byte 'for the event
                                            arrayStartIndex += 2
                                            For i = 0 To numOfFlagInfo - 1
                                                'array{teamID, isFlagCaptured, carrierID}
                                                Dim teamID = recv(arrayStartIndex)
                                                teamFlagsUpdated(i) = teamID
                                                Teams(teamID).FlagIsCaptured = CBool(recv(arrayStartIndex + 1))
                                                If Teams(teamID).FlagIsCaptured Then
                                                    Teams(teamID).FlagCarriedByPlayerID = recv(arrayStartIndex + 2)
                                                    Console.WriteLine("flag" & teamID & " is carried by player " & Teams(teamID).FlagCarriedByPlayerID)
                                                Else
                                                    Console.WriteLine("flag" & teamID & " is on the base")
                                                End If
                                                RaiseEvent Gameplay_CTF_Flag_Update_Event(teamID, Teams(teamID).FlagIsCaptured, Teams(teamID).FlagCarriedByPlayerID, UserData)
                                                arrayStartIndex += 3
                                            Next
                                            RaiseEvent Gameplay_CTF_Flags_Update_Event(teamFlagsUpdated, UserData)
                                        Else
                                            arrayStartIndex += 1 ' point to after the unknownByte
                                        End If
                                        subpacketLength = arrayStartIndex - subpacketStartIndex - 1 'set subpacket length manually since it's not static
                                    Case &H1F 'CTF info
                                        subpacketLength = 3 'for plus?
                                    Case &H22 'Bullet(Plus)??
                                        ' subpacketLength = 16 'for plus?
                                        ' Dim bulletX As Short = BitConverter.ToInt16(recv, +4)
                                        ' Dim bulletY As Short = BitConverter.ToInt16(recv, +6)
                                        ' Dim ammo As Byte = recv(subpacketStartIndex + 16)
                                End Select
                                subpacketStartIndex += 1 + subpacketLength
                            End While
                        Case &H9
                            If recv.Length > 8 Then
                                Dim Packet9SendBack As Byte() = {0, 0, &H9, UDPCount, 0, recv(5), recv(6), recv(7), recv(8)}
                                UDPchecksum(Packet9SendBack)
                                If ExtraLatency > 0 Then
                                    System.Threading.Thread.Sleep(ExtraLatency)
                                End If
                                Winsock2SendData(Packet9SendBack)
                            End If
                    End Select
                End If
            Catch ex As Exception
                RaiseEvent Error_Event(False, 3, ex.Message, UserData)
            Finally
            End Try
        End Sub

        Shared GAMEPLAY_SUBPACKET_LENGTH As Integer() = {0, 3, 3, 1, 2, 1, 1, 7, 12, 1, 2, 4, 4, 1, 4, 2, 5, 2, 2, 2, 2, 3, 4, 2, 3, 4, 3, 9, 10, 3, 4, 2, 7}
        Shared Function GetGameplaySubpacketLength(subpacketID As Byte)
            If subpacketID < GAMEPLAY_SUBPACKET_LENGTH.Length Then
                Return GAMEPLAY_SUBPACKET_LENGTH(subpacketID)
            Else
                Return 0
            End If
        End Function
        Shared GAMEPLAY_SUBPACKET_LENGTH_PLUS As Integer() = {0, 3, 3, 1, 2, 1, 1, 7, 12, 1, 3, 4, 4, 1, 10, 2, 2, 2, 2, 2, 2, 3, 4, 2, 3, 4, 3, 9, 10, 3, 4, 2, 7}
        Shared Function GetPlusGameplaySubpacketLength(subpacketID As Byte)
            If subpacketID < GAMEPLAY_SUBPACKET_LENGTH_PLUS.Length Then
                Return GAMEPLAY_SUBPACKET_LENGTH_PLUS(subpacketID)
            Else
                Return 0
            End If
        End Function

        Dim antiSpamSecondsElapsed As Byte = 0
        Private Sub every1SecTimerTick(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs)
            'UDP
            If udpTimerState Then
                If ConnectionTimeOut <> 0 Then
                    ConnectionTimeOut -= 1
                Else
                    udpTimerState = False
                    WinsockClose(&H9)
                End If
            End If

            'Anti-Spam
            If AntiSpam <> False Then
                antiSpamSecondsElapsed += 1
                If antiSpamSecondsElapsed >= _antiSpamClearSeconds Then
                    antiSpamSecondsElapsed = 0
                    For i As Byte = 0 To JJ2ClientsSockInfo.Length - 1
                        If ActiveClients(i) Then
                            If JJ2ClientsSockInfo(i) IsNot Nothing Then
                                JJ2ClientsSockInfo(i).AntiSpamCount = 0
                            End If
                        End If
                    Next
                End If
            End If
        End Sub
        Private Sub AnimTimerTick(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs)
            If _connected Then
                posPacket(posPacket.Length - 3) += 1
                If posPacket(posPacket.Length - 3) = 5 Then
                    posPacket(posPacket.Length - 3) = 0
                End If
                ''Anti Idle
                If posPacket(5) = &HB5 Then
                    posPacket(5) = &HB6
                Else
                    posPacket(5) = &HB5
                End If
                Winsock2SendData(posPacket)
            End If
        End Sub

        Private Sub Winsock1SendData(ByVal data As Byte())
            Try
                Winsock1.Send(data)
                TotalBytesSent += data.Length
            Catch objDisEx As ObjectDisposedException
                If _connected Then
                    WinsockClose(7)
                End If
            Catch sockEx As SocketException
                ' MsgBox("Winsock1SendData")
                WinsockClose(7)
            End Try
        End Sub
        Private Sub Winsock2SendData(ByVal data As Byte())
            Try
                UDPchecksum(data)
                Winsock2.Client.SendTo(data, _remoteEP)
                TotalBytesSent += data.Length
            Catch exSock As SocketException
                '  Console.WriteLine("UDP Winsock Error.")
            End Try
        End Sub
        Dim posPacket As Byte() = {&H3D, &HA7, &H1, &H0, &H1, &HB5, &H0, &H0, &H0, &H80, &H0, &H0, &H1E, &H0, &H0, &H0}
        Private Sub ChangeLevel(levelName As String, levelCRC32 As Int32) 'Loading screen equivalent
            _currentLevelName = levelName
            _levelCRC32 = levelCRC32

            'clean
            For i As Byte = 0 To Players.Length - 1
                If Players(i) IsNot Nothing Then Players(i).ClearStats(_plusServer)
            Next
            _isFirstScoreUpdate = True
            _isFirstGameState = True

            Dim replyPacket As Byte()
            If Not PlusServer Then
                _gameInProgress = True
                replyPacket = {2, &H1A}
                udpTimerState = True
            Else
                _gameInProgress = False
                _scriptsEnabled = _plusOnly
                Dim joiningDataPlus4 As Byte() = {&H6, &H1A, PlusCheck(0), PlusCheck(1), PlusCheck(2), PlusCheck(3)}
                replyPacket = joiningDataPlus4
            End If
            _cycling = False
            Winsock1SendData(replyPacket)
            If _ID <> &HFF Then
                If Players(_ID) IsNot Nothing Then
                    RaiseEvent Level_Initialized_Event(_currentLevelName, Players(_ID).Name, _ID, socketID, UserData)
                Else
                    RaiseEvent Error_Event(False, 1000, "Your player info (index=" & _ID & ") is NULL.", UserData)
                    RaiseEvent Level_Initialized_Event(_currentLevelName, "", _ID, socketID, UserData)
                End If
            End If
        End Sub

        Public Sub SendJJ2PlusNetworkStream(ByVal streamData As Byte(), ByVal scriptModuleId As Byte)
            'max length is 252 temp
            If streamData.Length <= 252 Then
                Dim sendData(streamData.Length + 2) As Byte
                sendData(0) = CByte(sendData.Length)
                sendData(1) = &H57
                sendData(2) = scriptModuleId
                Array.Copy(streamData, 0, sendData, 3, streamData.Length)
                Winsock1SendData(sendData)
            End If
        End Sub
        Public Function GetScriptModuleId(ByVal scriptName As String) As Byte
            If scriptModules.ContainsKey(scriptName) Then
                Return scriptModules(scriptName)
            Else
                Return &HFF
            End If
        End Function
        Private Function Combine2Arrays(ByVal a1 As Byte(), ByVal a2 As Byte()) As Byte()
            Dim list As New List(Of Byte)()

            For Each a As Byte In a1
                list.Add(a)
            Next

            For Each a As Byte In a2
                list.Add(a)
            Next

            Return list.ToArray()

        End Function
        Private Function Check_Client_connection(ByRef sckt As Socket)
            Dim ClientStatus As Boolean
            If sckt.Connected Then
                If sckt.Available = 0 And sckt.Poll(1000, SelectMode.SelectRead) = True Then
                    ClientStatus = False
                Else
                    ClientStatus = True
                End If
            Else
                ClientStatus = False
            End If
            Return ClientStatus
        End Function
        Private Shared Function UDPchecksum(ByVal buffer As Byte())
            '3.UDP checksum
            Dim x As Integer = 1, y As Integer = 1
            For i As Integer = 2 To buffer.Length - 1
                x += buffer(i)
                y += x
            Next
            buffer(0) = CByte(x Mod 251)
            buffer(1) = CByte(y Mod 251)
        End Function
        Private Function GetAdminSocketID(ByVal int As Byte()) As Byte
            Dim result As Byte = &HFF
            Dim Int2 As Integer
            If int.Length = 2 Then
                Int2 = BitConverter.ToInt16(int, 0)
            ElseIf int.Length = 4 Then
                Int2 = BitConverter.ToInt32(int, 0)
            ElseIf int.Length = 8 Then
                Int2 = BitConverter.ToInt64(int, 0)
            Else
                GoTo giveResult
            End If
            Select Case Int2
                Case 0
                    result = &HFE
                Case 2
                    result = 1
                Case 6
                    result = 2
                Case 10
                    result = 3
                Case 26
                    result = 4
                Case 58
                    result = 5
                Case 90
                    result = 6
                Case 218
                    result = 7
                Case 474
                    result = 8
                Case 986
                    result = 9
                Case 2010
                    result = 10
            End Select
giveResult:
            Return result
        End Function



        Public ReadOnly Property GameStarted As Boolean
            Get
                Return _gameStarted
            End Get
        End Property
        ''' <summary>
        ''' Gets or sets spectator mode state of local player. If set, the client will send request to the server.
        ''' </summary>
        Public Property IsSpectating As Boolean
            Set(value As Boolean)
                If _connected Then
                    Dim specPacket As Byte() = {&H3, &H42, &H20}
                    If value Then
                        specPacket(2) += 1
                    End If
                    Winsock1SendData(specPacket)
                End If
            End Set
            Get
                If JJ2ClientsSockInfo(socketID) IsNot Nothing Then
                    Return JJ2ClientsSockInfo(socketID).Spectating
                Else
                    Return False
                End If
            End Get
        End Property
        Public Function SendData(ByVal data As Byte(), ByVal protocol As Network_Protocol) As Boolean
            If _connected Then
                If protocol = 0 Then
                    Winsock1SendData(data)
                ElseIf protocol = 1 Then
                    Winsock2SendData(data)
                Else
                    Return False
                    Exit Function
                End If
                Return True
            Else
                Return False
            End If
        End Function
        Public Function SendMessage(ByVal msg As String) As String
            If _connected And socketID <> &HFF Then
                Try
                    Dim messagePacket As Byte() = {0, &H1B, socketID, &H20}
                    msg = System.Text.Encoding.Default.GetString(messagePacket) & msg
                    messagePacket = DefaultEncoding.GetBytes(msg)
                    messagePacket(0) = messagePacket.Length
                    Winsock1SendData(messagePacket)
                    Return vbNullString
                Catch ex As Exception
                    Return "Unknown error."
                End Try
            Else
                Return "Client is not connected."
            End If
        End Function
        Public Function Whisper(ByVal msg As String, ByVal recvPlayer As Byte)
            Dim whispPacketHeader As Byte() = {&H0, &H58, recvPlayer}
            Dim msgBytes = DefaultEncoding.GetBytes(msg)
            Dim copyLength = msgBytes.Length
            If copyLength > 252 Then
                copyLength = 252
            End If
            Dim whispPacket(whispPacketHeader.Length + copyLength - 1) As Byte
            Array.Copy(whispPacketHeader, whispPacket, whispPacketHeader.Length)
            Array.Copy(msgBytes, 0, whispPacket, 3, msgBytes.Length)
            whispPacket(0) = CByte(whispPacket.Length)
            Winsock1SendData(whispPacket)
        End Function


        ''' <summary>
        ''' Gets the list of connected players. Returns NULL if successful.
        Public Function GetPlayersList(ByRef numOfPlayers As Byte, ByRef playersNames As String(), ByRef playersIDs As Byte(), ByRef playersSocketIndexes As Byte(), ByRef playersTeam As Byte(), ByRef playersChar As Byte(), Optional ByVal ExcludeMe As Boolean = False, Optional ByVal ExcludeHost As Boolean = False) As String
            Dim result As String = ""
            If _connected Then
                If _ID <> &HFF And socketID <> &HFF Then
                    Dim playersNamesRes As New List(Of String)
                    Dim playersIDsRes As New List(Of Byte)
                    Dim playersSocketIndexesRes As New List(Of Byte)
                    Dim playersTeamRes As New List(Of Byte)
                    Dim playersCharRes As New List(Of Byte)
                    numOfPlayers = 0
                    Dim startingFrom As Byte
                    If ExcludeHost Then
                        startingFrom = 1
                    Else
                        startingFrom = 0
                    End If
                    For i As Byte = startingFrom To _connectionlimit
                        If JJ2ClientsSockInfo(i) IsNot Nothing Then
                            If JJ2ClientsSockInfo(i).NumOfPlayers <> 0 Then
                                For Each anID As Byte In JJ2ClientsSockInfo(i).PlayerID
                                    If anID <> &HFF And anID <= _connectionlimit Then
                                        If Not ((anID = _ID) And ExcludeMe) Then '(Me) NAND (ExcludeMe). This expression will exclude this client if ExcludeMe=TRUE.
                                            If Players(anID) IsNot Nothing Then
                                                playersSocketIndexesRes.Add(i)
                                                playersIDsRes.Add(anID)
                                                playersNamesRes.Add(Players(anID).Name)
                                                playersTeamRes.Add(Players(anID).Team)
                                                playersCharRes.Add(Players(anID).Character)
                                                numOfPlayers += 1
                                            End If
                                        End If
                                    End If
                                Next
                            End If
                        End If
                    Next
                    ReDim playersNames(numOfPlayers), playersIDs(numOfPlayers), playersSocketIndexes(numOfPlayers), playersTeam(numOfPlayers), playersChar(numOfPlayers)
                    playersSocketIndexes = playersSocketIndexesRes.ToArray
                    playersIDs = playersIDsRes.ToArray
                    playersTeam = playersTeamRes.ToArray
                    playersChar = playersCharRes.ToArray
                    playersNames = playersNamesRes.ToArray
                Else
                    result = "Connected but not initialized."
                End If
            Else
                result = "Not connected."
            End If
            Return result
        End Function
        Public Function GetNumOfSpectators(Optional ByVal ExcludeHost As Boolean = False) As Byte
            Dim result As Byte = 0
            If _connected Then
                If _ID <> &HFF And socketID <> &HFF Then
                    Dim startingFrom As Byte
                    If ExcludeHost Then
                        startingFrom = 1
                    Else
                        startingFrom = 0
                    End If
                    For i As Byte = startingFrom To _connectionlimit
                        If JJ2ClientsSockInfo(i) IsNot Nothing Then
                            If JJ2ClientsSockInfo(i).NumOfPlayers <> 0 Then
                                If JJ2ClientsSockInfo(i).Spectating <> False Then
                                    For Each anID As Byte In JJ2ClientsSockInfo(i).PlayerID
                                        If anID <> &HFF And anID <= _connectionlimit Then
                                            result += 1
                                        End If
                                    Next
                                End If
                            End If
                        End If

                    Next

                End If
            End If
            Return result
        End Function

        ''' <summary>
        ''' Gets the state of the connection to server.
        ''' </summary>
        Public ReadOnly Property Connected As Boolean
            Get
                Return _connected
            End Get
        End Property

        ''' <summary>
        ''' Gets the client's player name assigned by server.
        ''' </summary>
        Public ReadOnly Property CurrentName As String
            Get
                If _ID < Players.Length Then
                    If Players(_ID) IsNot Nothing Then
                        Return Players(_ID).Name
                        Exit Property
                    End If
                End If
                Return ""
            End Get
        End Property
        ''' <summary>
        ''' Gets total number of players on the server.
        ''' </summary>
        Public ReadOnly Property GetNumOfPlayers As Byte
            Get
                Dim result As Short = 0
                If _connected Then
                    For Each sInf As JJ2SocketInfo In JJ2ClientsSockInfo
                        If sInf IsNot Nothing Then
                            For Each id As Byte In sInf.playerID
                                If id <> &HFF Then
                                    result += 1
                                End If
                            Next
                            If result > 255 Then
                                result = 255
                            End If
                        End If
                    Next
                End If
                Return CByte(result)
            End Get
        End Property

        ''' <summary>
        ''' 
        ''' </summary>
        ''' <returns>Number of players on host machine</returns>
        Public ReadOnly Property GetNumOfHosts As Byte
            Get
                Dim result As Short = 0
                If _connected Then
                    If JJ2ClientsSockInfo(0) IsNot Nothing Then
                        For Each anId As Byte In JJ2ClientsSockInfo(0).playerID
                            If anId <> &HFF Then
                                result += 1
                            End If
                        Next
                        If result > 255 Then
                            result = 255
                        End If
                    End If

                End If
                Return CByte(result)
            End Get
        End Property
        ''' <summary>
        ''' Gets current game mode.
        ''' </summary>
        Public ReadOnly Property GameType As JJ2_Game_Type
            Get
                Return _gameType
            End Get
        End Property
        ''' <summary>
        ''' Gets current custom game mode of JJ2+. Retuens 0 if custom game is disabled.
        ''' </summary>
        Public ReadOnly Property CustomGameMode As JJ2_Custom_Game_Type
            Get
                Return _customGameMode
            End Get
        End Property
        ''' <summary>
        ''' Gets current number of scores required to win the game.
        ''' </summary>
        Public ReadOnly Property MaxScore As Integer
            Get
                Return _maxScore
            End Get
        End Property


        ''' <summary>
        ''' Gets wheather server is a special server (Gem Collecting Challenge).
        ''' </summary>
        Public ReadOnly Property SpecialServer As Boolean
            Get
                Return _specialServer
            End Get
        End Property
        ''' <summary>
        ''' Gets or sets the client's player name assigned by user (menu).
        ''' </summary>
        Public Property Name As String
            Get
                Return _name
            End Get
            Set(value As String)
                If value <> "" Then
                    If value.Length < 21 Then
                        _name = value
                    End If
                End If
            End Set
        End Property
        ''' <summary>
        ''' Gets your client ID among the clients in the server.
        ''' </summary>
        Public ReadOnly Property ClientID As Byte
            Get
                Return socketID
            End Get
        End Property
        ''' <summary>
        ''' Gets your in-game player ID (Zero-based).
        ''' </summary>
        Public ReadOnly Property PlayerID As Byte
            Get
                Return _ID
            End Get
        End Property
        ''' <summary>
        ''' Gets whether you are admin.
        ''' </summary>
        Public ReadOnly Property IsAdmin As Boolean
            Get
                If socketID <> &HFF Then
                    Return JJ2ClientsSockInfo(socketID).Admin
                Else
                    Return False
                End If
            End Get
        End Property
        ''' <summary>
        ''' Gets whether spectator mode is ON.
        ''' </summary>
        Public ReadOnly Property SpectatorMode As Boolean
            Get
                If socketID <> &HFF Then
                    Return CBool(JJ2ClientsSockInfo(socketID).Spectating)
                Else
                    Return False
                End If
            End Get
        End Property

        ''' <summary>
        ''' Gets whether server is hosted using JJ2+ ot not.
        ''' </summary>
        Public ReadOnly Property PlusServer As Boolean
            Get
                Return _plusServer
            End Get
        End Property

        ''' <summary>
        ''' Gets server address.
        ''' </summary>
        Public ReadOnly Property RemoteAddress As String
            Get
                If _connected Then
                    Return _serverAddress
                Else
                    Return ""
                End If
            End Get
        End Property
        ''' <summary>
        ''' Gets server IP address.
        ''' </summary>
        Public ReadOnly Property RemoteIP As String
            Get
                If Connected Then
                    Return _remoteEP.Address.ToString()
                Else
                    Return ""
                End If
            End Get
        End Property
        ''' <summary>
        ''' Gets server port number.
        ''' </summary>
        Public ReadOnly Property RemotePort As UShort
            Get
                If Connected Then
                    Return CUShort(_remoteEP.Port)
                Else
                    Return 0
                End If
            End Get
        End Property
        ''' <summary>
        ''' Returns 0 if no local UDP port assigned otherwise the return value is the local UDP port number.
        ''' </summary>
        Public ReadOnly Property UDPLocalPort As UShort
            Get
                If Winsock2 IsNot Nothing Then
                    Return Winsock2.Client.LocalEndPoint.ToString.Split(":")(1)
                Else
                    Return 0
                End If
            End Get
        End Property
        ''' <summary>
        ''' Gets the state of end of level screen (black screen, winner is...)
        ''' </summary>
        Public ReadOnly Property IsLevelCycling As Boolean
            Get
                Return _cycling
            End Get
        End Property
        ''' <summary>
        ''' Gets the name of the current level hosted in remote server.
        ''' </summary>
        Public ReadOnly Property CurrentLevelName As String
            Get
                Return _currentLevelName
            End Get
        End Property
        ''' <summary>
        ''' Gets JJ2+ Game state. returns as JJ2Plus_Game_State.
        ''' </summary>
        Public ReadOnly Property GetGameState As JJ2Plus_Game_State
            Get
                Return _gameState
            End Get
        End Property
        ''' <summary>
        ''' Gets a value indicating whether there is ongoing game. This value can be used to determine if players can use "/ready" command.
        ''' </summary>
        Public ReadOnly Property GameInProgress As Boolean
            Get
                Return _gameInProgress
            End Get
        End Property

        ''' <summary>
        ''' Gets if only JJ2+ users are allowed to join the server.
        ''' </summary>
        Public ReadOnly Property PlusOnly As Boolean
            Get
                Return _plusOnly
            End Get
        End Property
        ''' <summary>
        ''' Gets the state of scripts.
        ''' </summary>
        ''' <returns>Returns the state of PlusOnly at level load time.</returns>
        Public ReadOnly Property ScriptsEnabled As Boolean
            Get
                Return _scriptsEnabled
            End Get
        End Property
    End Class
End Namespace


