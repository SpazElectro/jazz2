//This example will show you how to:
//- Add jj2 client events.
//- Connect to & disconnect from a JJ2 server.
//- Log player messages, joining/leaving notifications, and loaded level name.

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Threading;
using JJ2ClientLib.JJ2;

using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Windows.Forms;

namespace jj2livestream
{
    internal class Program
    {
        public static JJ2Client jj2 = new JJ2Client();   

        static byte[][] CaptureScreen()
        {
            // Capture the screen
            using (Bitmap screenCapture = new Bitmap(Screen.PrimaryScreen.Bounds.Width, Screen.PrimaryScreen.Bounds.Height))
            {
                using (Graphics g = Graphics.FromImage(screenCapture))
                {
                    g.CopyFromScreen(Point.Empty, Point.Empty, Screen.PrimaryScreen.Bounds.Size);
                }

                // Create a two-dimensional byte array to store encoded pixel colors
                byte[][] encodedPixels = new byte[screenCapture.Width][];
                
                for (int x = 0; x < screenCapture.Width; x++)
                {
                    encodedPixels[x] = new byte[screenCapture.Height * 3]; // 3 bytes for R, G, and B per pixel
                    
                    for (int y = 0; y < screenCapture.Height; y++)
                    {
                        Color pixelColor = screenCapture.GetPixel(x, y);
                        byte[] colorBytes = EncodeColor(pixelColor);
                        
                        // Store the encoded color bytes in the array
                        encodedPixels[x][y * 3] = colorBytes[0]; // Red
                        encodedPixels[x][y * 3 + 1] = colorBytes[1]; // Green
                        encodedPixels[x][y * 3 + 2] = colorBytes[2]; // Blue
                    }
                }

                return encodedPixels;
            }
        }

        static byte[] EncodeColor(Color color)
        {
            // Define a custom palette of colors (you can add more colors as needed)
            Color[] palette = new Color[]
            {
                Color.Black,
                Color.Red,
                Color.Green,
                Color.Blue,
                Color.Yellow,
                Color.Magenta,
                Color.Cyan,
                Color.White
            };

            byte[] colorBytes = new byte[3]; // Exclude alpha channel

            // Find the closest color in the palette to the input color
            Color closestColor = FindClosestColor(color, palette);

            // Encode the closest color components into bytes
            colorBytes[0] = closestColor.R;
            colorBytes[1] = closestColor.G;
            colorBytes[2] = closestColor.B;

            return colorBytes;
        }

        static Color FindClosestColor(Color targetColor, Color[] palette)
        {
            Color closestColor = palette[0];
            int closestDistance = ColorDistance(targetColor, palette[0]);

            for (int i = 1; i < palette.Length; i++)
            {
                int distance = ColorDistance(targetColor, palette[i]);
                if (distance < closestDistance)
                {
                    closestColor = palette[i];
                    closestDistance = distance;
                }
            }

            return closestColor;
        }

        static int ColorDistance(Color color1, Color color2)
        {
            int deltaR = color1.R - color2.R;
            int deltaG = color1.G - color2.G;
            int deltaB = color1.B - color2.B;
            return deltaR * deltaR + deltaG * deltaG + deltaB * deltaB;
        }

        static byte[] ConvertToByteArray(byte[][] encodedPixels)
        {
            // Flatten the two-dimensional array into a single-dimensional array
            return encodedPixels.SelectMany(row => row).ToArray();
        }

        public static void Main(string[] args)
        {
            jj2.Level_Initialized_Event += onLevelInitialize;
            jj2.Connected_Event += onConnect;
            jj2.Disconnected_Event += onDisconnect;
            jj2.Failed_To_Connect_Event += onConnectFail;

            //Connect to JJ2 server       
            string serverAddress = "localhost"; //127.0.0.1 (This machine)
            UInt16 serverPort = 10052; //default JJ2 game port
            Console.WriteLine(String.Format("* * * Connecting to [{0}]...", serverAddress));
            jj2.JoinServer(serverAddress, null, "Livestreamer", serverPort);

            //wait for user to press ESC key
            while (true)
            {
                // if (Console.ReadKey().Key == ConsoleKey.Escape)
                // {
                //     //Disconnect and release resources
                //     jj2.Leave();
                //     jj2.Dispose();
                //     break;
                // }

                // get all pixels onto the screen and put into an array and send thro jj2.sendpluspacketstream
                // byte[] pixels = ConvertToByteArray(CaptureScreen());
                byte[] testBytes = new byte[5];
                testBytes[0] = 1;
                testBytes[1] = 2;
                testBytes[2] = 3;
                testBytes[3] = 4;
                testBytes[4] = 5;
                
                jj2.SendJJ2PlusNetworkStream(testBytes, 0);

                System.Threading.Thread.Sleep(10000);
            }
        }

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
    }
}
