using JJ2ClientLib;
using JJ2ClientLib.JJ2;

public static class GameClient {
    public static JJ2Client client = new JJ2Client();
    public static string log = "";

    public static void joinGame(string addr, ushort port) {
        client.JoinServer(addr, null, "Admin", port);
    }

    // jj2 connection events
    private static void onLevelInitialize(string levelName, string yourName, byte yourID, byte yourSocketIndex, object user)
    {
        string line = string.Format("* * * Level begin [{0}] at [{1}]", levelName, DateTime.Now.ToString());
        Console.WriteLine(line);
    }

    private static void onConnect(string serverIPAddrees, string serverAddress, ushort serverPort, object user)
    {
        string line = string.Format("* * * Connected to [{0}:{1}]", serverAddress, serverPort.ToString());
        Console.WriteLine(line);
    }

    private static void onDisconnect(JJ2_Disconnect_Message disconnectMessage, string serverIPAddrees, string serverAddress, ushort serverPort, object user)
    {
        string line = string.Format("* * * Disconnected from [{0}:{1}] at [{2}] with reason [{3}]", serverAddress, serverPort.ToString(), DateTime.Now.ToString(), disconnectMessage.ToString());
        Console.WriteLine(line);
    }

    private static void onConnectFail(string serverAddress, ushort serverPort, object user)
    {
        string line = string.Format("* * * Unable to connect to [{0}:{1}] ", serverAddress, serverPort.ToString());
        Console.WriteLine(line);
    }

    // util
    public static void logMessage(string msg) {
        Console.WriteLine(msg);
        log += msg;
        updateLog();
    }

    public static void updateLog() {
        // do some packet stuff or something with socket.io idk
    }
}