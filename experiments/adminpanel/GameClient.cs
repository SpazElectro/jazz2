using System.Net.WebSockets;
using Newtonsoft.Json;
using JJ2ClientLib.JJ2;
using Newtonsoft.Json.Linq;

public class GameClient {
    public JJ2Client client = new JJ2Client();
    public WebSocketHandler wsHandler;

    public string _addr = "";
    public ushort _port = 0;

    public GameClient(WebSocketHandler wsHandler) {
        RegisterJJ2Events();
        this.wsHandler = wsHandler;
    }

    public void RegisterJJ2Events() {
        client.Console_Message_Recveived_Event += OnConsoleMessageReceive;
        client.Message_Received_Event += OnMessageReceive;
        client.Player_Joined_Event += OnPlayerJoin;
        client.Player_Left_Event += OnPlayerLeft;
        client.Level_Initialized_Event += OnLevelInitialize;
        client.Connected_Event += OnConnect;
        client.Disconnected_Event += OnDisconnect;
        client.Failed_To_Connect_Event += OnConnectFail;
    }

    public void JoinGame(string addr, ushort port) {
        _addr = addr;
        _port = port;

        client.JoinServer(addr, null, "Admin", port);
    }

    public void RejoinLastGame() {
        if(_addr != "" && _port != 0)
            client.JoinServer(_addr, null, "Admin", _port);
        else Console.WriteLine("Tried to GameClient.RejoinLastGame while there wasn't any previous connection!");
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
                ClientID = playersIDs[sock],
                Team = playersTeam[sock], // dunno if this works
                Roasts = client.Players[sock].Roasts,
                Deaths = client.Players[sock].Deaths,
                Color =  client.Players[sock].Color
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
        SendPlayers();
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
        SendPlayers();
    }

    public void OnLevelInitialize(string levelName, string yourName, byte yourID, byte yourSocketIndex, object user) => LogMessage(string.Format("* * * Level begin [{0}] at [{1}]", levelName, DateTime.Now.ToString()));
    public void OnConnect(string serverIPAddrees, string serverAddress, ushort serverPort, object user) {
        LogMessage(string.Format("* * * Connected to [{0}:{1}]", serverAddress, serverPort.ToString()));

        SendPlayers();
    }

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

    public void SendPlayers() {
        List<JJ2Player> players = GetPlayers();
        JArray playersArray = new JArray();

        foreach (JJ2Player player in players) {
            playersArray.Add(new JObject(
                new JProperty("name", player.Name),
                new JProperty("character", player.Character == JJ2_Character.JAZZ ? "jazz" : (player.Character == JJ2_Character.SPAZ ? "spaz" : (player.Character == JJ2_Character.LORI ? "lori" : ("UNKNOWN_" + player.Character.ToString())))),
                new JProperty("fur", new JArray(player.Color)), // Wrap player.Color in a JArray
                new JProperty("sockId", player.ClientID),
                new JProperty("roasts", player.Roasts),
                new JProperty("deaths", player.Deaths),
                new JProperty("team", player.Team)
            ));
        }

        wsHandler.Broadcast(async (x) => {
            await x.SendAsync(WebSocketHandler.StringToArraySegment("players:" + playersArray.ToString(Formatting.None)), WebSocketMessageType.Text, true, CancellationToken.None);
        }).Wait();
    }

}