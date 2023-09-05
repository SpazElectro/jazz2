using System.Net.WebSockets;
using JJ2ClientLib.JJ2;

public static class GameClient {
    public static JJ2Client client = new JJ2Client();
    public static WebSocketHandler wsHandler = new WebSocketHandler("http://127.0.0.1:1/");

    public static void joinGame(string addr, ushort port, WebSocketHandler wsHandler) {
        client.Console_Message_Recveived_Event += onConsoleMessageReceive;
        client.Message_Received_Event += onMessageReceive;
        client.Player_Joined_Event += onPlayerJoin;
        client.Player_Left_Event += onPlayerLeft;

        client.Level_Initialized_Event += onLevelInitialize;
        client.Connected_Event += onConnect;
        client.Disconnected_Event += onDisconnect;
        client.Failed_To_Connect_Event += onConnectFail;

        client.JoinServer(addr, null, "Admin", port);
        
        GameClient.wsHandler = wsHandler;
    }

    public static List<JJ2Player> getPlayers() {
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

    public static void onConsoleMessageReceive(string msg, byte msgType, object user)
    {
        addTimeToString(ref msg);
        logMessage(msg);
    }

    public static void onMessageReceive(string msg, string playerName, byte team, byte playerID, byte playerSocketIndex, object user)
    {
        if (team > 0)
        {
            playerName = '[' + playerName + ']';
        }

        string line = string.Format("{0}: {1}", playerName, msg);
        addTimeToString(ref line);

        logMessage(line);
    }

    public static void onPlayerJoin(string playerName, byte playerID, byte socketIndex, byte character, byte team, object user)
    {
        string line = string.Format("{0} has joined the game", playerName);
        addTimeToString(ref line);

        logMessage(line);
    }

    public static void onPlayerLeft(string playerName, JJ2_Disconnect_Message disconnectMessage, byte playerID, byte socketIndex, object user)
    {
        string line = string.Format("{0} has left the game", playerName);
        addTimeToString(ref line);

        logMessage(line);
    }

    public static void onLevelInitialize(string levelName, string yourName, byte yourID, byte yourSocketIndex, object user) {
        logMessage(string.Format("* * * Level begin [{0}] at [{1}]", levelName, DateTime.Now.ToString()));
    }

    public static void onConnect(string serverIPAddrees, string serverAddress, ushort serverPort, object user) {
        logMessage(string.Format("* * * Connected to [{0}:{1}]", serverAddress, serverPort.ToString()));
    }

    public static void onDisconnect(JJ2_Disconnect_Message disconnectMessage, string serverIPAddrees, string serverAddress, ushort serverPort, object user) {
        logMessage(string.Format("* * * Disconnected from [{0}:{1}] at [{2}] with reason [{3}]", serverAddress, serverPort.ToString(), DateTime.Now.ToString(), disconnectMessage.ToString()));
    }

    public static void onConnectFail(string serverAddress, ushort serverPort, object user) {
        logMessage(string.Format("* * * Unable to connect to [{0}:{1}] ", serverAddress, serverPort.ToString()));
    }

    // util
    public static void logMessage(string msg) {
        Console.WriteLine(msg);
        wsHandler.Broadcast(async (client) => {
             await client.SendAsync(WebSocketHandler.StringToArraySegment("message: " + msg), WebSocketMessageType.Text, true, CancellationToken.None);
        }).Wait();
    }

    public static void addTimeToString(ref string s)
    {
        DateTime d = DateTime.Now;
        s = string.Format("[{0}:{1}:{2}] {3}", d.Hour.ToString("00"), d.Minute.ToString("00"), d.Second.ToString("00"), s);
    }
}