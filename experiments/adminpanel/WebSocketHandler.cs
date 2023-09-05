using System;
using System.Collections.Concurrent;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

public class WebSocketHandler
{
    private readonly HttpListener _httpListener;
    private readonly ConcurrentDictionary<string, WebSocket> _clients = new ConcurrentDictionary<string, WebSocket>();

    public WebSocketHandler(string url)
    {
        _httpListener = new HttpListener();
        _httpListener.Prefixes.Add(url); // Define the URL where the WebSocket server will listen
    }

    public static ArraySegment<byte> StringToArraySegment(string input)
    {
        byte[] byteArray = Encoding.UTF8.GetBytes(input);
        return new ArraySegment<byte>(byteArray, 0, byteArray.Length);
    }

    public static string ArraySegmentToString(ArraySegment<byte> arraySegment)
    {
        byte[]? byteArray = arraySegment.Array;
        int offset = arraySegment.Offset;
        int count = arraySegment.Count;
        
        if (byteArray != null)
        {
            return Encoding.UTF8.GetString(byteArray, offset, count);
        }
        
        return string.Empty;
    }

    public static string ByteArrayToString(byte[] byteArray)
    {
        if (byteArray != null)
        {
            return Encoding.UTF8.GetString(byteArray);
        }

        return string.Empty;
    }

    public async Task Start()
    {
        _httpListener.Start();
        Console.WriteLine("WebSocket server is running...");

        while (true)
        {
            HttpListenerContext context = await _httpListener.GetContextAsync();
            _ = Task.Run(() => HandleWebSocketAsync(context));
        }
    }

    public delegate Task BroadcastDelegate(WebSocket client);
    public async Task Broadcast(BroadcastDelegate delg)
    {
        foreach (var client in _clients.Values)
        {
            if (client.State == WebSocketState.Open)
            {
                await delg(client);
                // await client.SendAsync(msg,
                //                 WebSocketMessageType.Text, true, CancellationToken.None);
            }
        }
    }

    private async Task HandleWebSocketAsync(HttpListenerContext context)
    {
        if (context.Request.IsWebSocketRequest)
        {
            HttpListenerWebSocketContext webSocketContext = await context.AcceptWebSocketAsync(null);

            WebSocket webSocket = webSocketContext.WebSocket;

            // Generate a unique client ID based on their socket hash code
            string clientId = webSocket.GetHashCode().ToString();

            // Add the client to the dictionary
            _clients.TryAdd(clientId, webSocket);

            Task receiveTask = Task.Run(async () =>
            {
                while (webSocket.State == WebSocketState.Open)
                {
                    try
                    {
                        byte[] buffer = new byte[1024];
                        WebSocketReceiveResult result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);

                        if (result.MessageType == WebSocketMessageType.Text)
                        {
                            string receivedMessage = Encoding.UTF8.GetString(buffer, 0, result.Count);
                            Console.WriteLine($"Received from client {clientId}: {receivedMessage}");

                            // Find the first occurrence of ":"
                            int firstIndex = receivedMessage.IndexOf(':');

                            if (firstIndex != -1)
                            {
                                string type = receivedMessage.Substring(0, firstIndex).Trim();
                                string content = receivedMessage.Substring(firstIndex + 1).Trim();

                                Console.WriteLine($"Type: {type}");
                                Console.WriteLine($"Content: {content}");

                                if (type == "message")
                                    await Broadcast(async (client) => await client.SendAsync(StringToArraySegment("message:User: " + content), WebSocketMessageType.Text, true, CancellationToken.None));
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"WebSocket error for client {clientId}: {ex.Message}");
                        break;
                    }
                }

                // Remove the client from the dictionary when the WebSocket is closed
                _clients.TryRemove(clientId, out _);
            });

            await receiveTask;
        }
    }
}
