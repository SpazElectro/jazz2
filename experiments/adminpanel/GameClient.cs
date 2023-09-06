using System.Net.WebSockets;
using JJ2ClientLib.JJ2;

public class GameClient {
    public JJ2Client client = new JJ2Client();
    public WebSocketHandler wsHandler;

    public GameClient(WebSocketHandler wsHandler) {
        client.Console_Message_Recveived_Event += OnConsoleMessageReceive;
        client.Message_Received_Event += OnMessageReceive;
        client.Player_Joined_Event += OnPlayerJoin;
        client.Player_Left_Event += OnPlayerLeft;
        client.Level_Initialized_Event += OnLevelInitialize;
        client.Connected_Event += OnConnect;
        client.Disconnected_Event += OnDisconnect;
        client.Failed_To_Connect_Event += OnConnectFail;
        
        this.wsHandler = wsHandler;
    }

    public void JoinGame(string addr, ushort port) {
        client.JoinServer(addr, null, "Admin", port);
    }

    public List<JJ2Player> GetPlayers() {
        List<JJ2Player> players = new List<JJ2Player>();

        byte numOfPlayers = 0;
        string[] playersNames = { };
        byte[] playersIDs = { };
        byte[] playersSocketIndexes = { };
        byte[] playersTeam = { };
        byte[] playersChar = { };

        client.GetPlayersList(
            ref numOfPlayers,
            ref playersNames,
            ref playersIDs,
            ref playersSocketIndexes,
            ref playersTeam,
            ref playersChar,
            false, false
        );

        playersSocketIndexes.ToList().ForEach(sock =>
        {
            JJ2Player player = new JJ2Player(sock, playersChar[sock], playersTeam[sock], new JJ2SocketInfo())
            {
                Name = playersNames[sock],
                ClientID = playersIDs[sock]
            };

            players.Add(player);
        });

        return players;
    }

    // jj2 connection events

    public void OnConsoleMessageReceive(string msg, byte msgType, object user)
    {
        AddTimeToString(ref msg);
        LogMessage(msg);
    }

    public void OnMessageReceive(
        string msg,
        string playerName,
        byte team,
        byte playerID,
        byte playerSocketIndex,
        object user)
    {
        if (team > 0)
        {
            playerName = '[' + playerName + ']';
        }

        string line = string.Format("{0}: {1}", playerName, msg);
        AddTimeToString(ref line);

        LogMessage(line);
    }

    public void OnPlayerJoin(
        string playerName,
        byte playerID,
        byte socketIndex,
        byte character,
        byte team,
        object user)
    {
        string line = string.Format("{0} has joined the game", playerName);
        AddTimeToString(ref line);

        LogMessage(line);
    }

    public void OnPlayerLeft(string playerName,
        JJ2_Disconnect_Message disconnectMessage,
        byte playerID,
        byte socketIndex,
        object user)
    {
        string line = string.Format("{0} has left the game", playerName);
        AddTimeToString(ref line);

        LogMessage(line);
    }

    public void OnLevelInitialize(string levelName, string yourName, byte yourID, byte yourSocketIndex, object user) => LogMessage(string.Format("* * * Level begin [{0}] at [{1}]", levelName, DateTime.Now.ToString()));
    public void OnConnect(string serverIPAddrees, string serverAddress, ushort serverPort, object user) => LogMessage(string.Format("* * * Connected to [{0}:{1}]", serverAddress, serverPort.ToString()));
    public void OnDisconnect(JJ2_Disconnect_Message disconnectMessage, string serverIPAddrees, string serverAddress, ushort serverPort, object user) => LogMessage(string.Format("* * * Disconnected from [{0}:{1}] at [{2}] with reason [{3}]", serverAddress, serverPort.ToString(), DateTime.Now.ToString(), disconnectMessage.ToString()));
    public void OnConnectFail(string serverAddress, ushort serverPort, object user) => LogMessage(string.Format("* * * Unable to connect to [{0}:{1}] ", serverAddress, serverPort.ToString()));

    // util
    public void LogMessage(string msg) {
        Console.WriteLine(msg);
        wsHandler.Broadcast(async (client) => {
             await client.SendAsync(WebSocketHandler.StringToArraySegment("message: " + msg), WebSocketMessageType.Text, true, CancellationToken.None);
        }).Wait();
    }

    public static void AddTimeToString(ref string s)
    {
        DateTime d = DateTime.Now;
        s = string.Format("[{0}:{1}:{2}] {3}", d.Hour.ToString("00"), d.Minute.ToString("00"), d.Second.ToString("00"), s);
    }
}