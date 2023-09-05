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
                                {
                                    // Broadcast the message to all connected clients
                                    foreach (var client in _clients.Values)
                                    {
                                        if (client.State == WebSocketState.Open)
                                        {
                                            await client.SendAsync(new ArraySegment<byte>(buffer, 0, result.Count),
                                                                    WebSocketMessageType.Text, true, CancellationToken.None);
                                        }
                                    }
                                }
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
