//This example will show you how to:
//- Add jj2 client events.
//- Connect to & disconnect from a JJ2 server.
//- Log player messages, joining/leaving notifications, and loaded level name.

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using JJ2ClientLib.JJ2;

namespace ChatLogger
{
    internal class Program
    {
        //-------------------------------------------------------//
        //Main program
        //-------------------------------------------------------//
        private static JJ2Client jj2 = new JJ2Client(); //JJ2 client.      
        static string logPath = ""; //Log file path.

        static void Main(string[] args)
        {
            logPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "chatlog.txt");

            //Initialize events
            jj2.Message_Received_Event += onMessageReceive;
            jj2.Player_Joined_Event += onPlayerJoin;
            jj2.Player_Left_Event += onPlayerLeft;
            jj2.Level_Initialized_Event += onLevelInitialize;
            jj2.Connected_Event += onConnect;
            jj2.Disconnected_Event += onDisconnect;
            jj2.Failed_To_Connect_Event += onConnectFail;

            //Connect to JJ2 server       
            string serverAddress = "localhost"; //127.0.0.1 (This machine)
            UInt16 serverPort = 10052; //default JJ2 game port
            Console.WriteLine(String.Format("* * * Connecting to [{0}]...", serverAddress));
            jj2.JoinServer(serverAddress, null, "TestBot", serverPort);

            //wait for user to press ESC key
            while (true)
            {
                if (Console.ReadKey().Key == ConsoleKey.Escape)
                {
                    //Disconnect and release resources
                    jj2.Leave();
                    jj2.Dispose();
                    break;
                }
            }
        }


        //-------------------------------------------------------//
        //Jazz Jackrabbit 2 client events
        //-------------------------------------------------------//
        private static void onMessageReceive(string msg, string playerName, byte team, byte playerID, byte playerSocketID, object user)
        {
            //This code is executed whenever a player message is received.
            if (team > 0) //If team chat
            {
                playerName = '[' + playerName + ']';
            }

            //Build string "[hh:mm:ss] name: message"
            string line = string.Format("{0}: {1}", playerName, msg);
            addTimeToString(ref line);

            //print and save line
            Console.WriteLine(line);
            saveLog(logPath, line);
        }

        private static void onPlayerJoin(string playerName, byte playerID, byte socketIndex, byte character, byte team, object user)
        {
            //This code is executed whenever a player joins the game.
            string line = string.Format("{0} has joined the game", playerName);
            addTimeToString(ref line);

            Console.WriteLine(line);
            saveLog(logPath, line);
        }

        private static void onPlayerLeft(string playerName, JJ2_Disconnect_Message disconnectMessage, byte playerID, byte socketIndex, object user)
        {
            //This code is executed whenever a player leaves the game.
            string line = string.Format("{0} has left the game", playerName);
            addTimeToString(ref line);

            Console.WriteLine(line);
            saveLog(logPath, line);
        }

        private static void onLevelInitialize(string levelName, string yourName, byte yourID, byte yourSocketIndex, object user)
        {
            //This code will get executed after level cycle.
            string line = string.Format("* * * Level begin [{0}] at [{1}]", levelName, DateTime.Now.ToString());
            Console.WriteLine(line);
            saveLog(logPath, line);
        }

        private static void onConnect(string serverIPAddrees, string serverAddress, ushort serverPort, object user)
        {
            //This code is executed when TCP connection to JJ2 server is established successfully.
            string line = string.Format("* * * Connected to [{0}:{1}]", serverAddress, serverPort.ToString());
            Console.WriteLine(line);
            saveLog(logPath, line);
        }

        private static void onDisconnect(JJ2_Disconnect_Message disconnectMessage, string serverIPAddrees, string serverAddress, ushort serverPort, object user)
        {
            //This code is executed when you get disconnected from the server.
            string line = string.Format("* * * Disconnected from [{0}:{1}] at [{2}] with reason [{3}]", serverAddress, serverPort.ToString(), DateTime.Now.ToString(), disconnectMessage.ToString());
            Console.WriteLine(line);
            saveLog(logPath, line);
        }

        //Failed_To_Connect_Event(ByVal serverAddress As String, ByVal serverPort As UShort, ByVal user As Object)
        private static void onConnectFail(string serverAddress, ushort serverPort, object user)
        {
            //This code will get executed if JJ2Client.JoinServer() fails.
            string line = string.Format("* * * Unable to connect to [{0}:{1}] ", serverAddress, serverPort.ToString());
            Console.WriteLine(line);
            saveLog(logPath, line);
        }


        //-------------------------------------------------------//
        //Extra functions
        //-------------------------------------------------------//
        private static void saveLog(string filePath, string txt)
        {
            try
            {
                StreamWriter w = File.AppendText(filePath);
                w.WriteLine(txt);
                w.Close();
            }
            catch (Exception ex)
            {
            }
        }

        private static void addTimeToString(ref string s)
        {
            DateTime d = DateTime.Now;
            s = string.Format("[{0}:{1}:{2}] {3}", d.Hour.ToString("00"), d.Minute.ToString("00"), d.Second.ToString("00"), s);
        }
    }
}
