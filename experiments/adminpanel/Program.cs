using Microsoft.AspNetCore.Http;
using System.IO;
using System.Threading.Tasks;

var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

static string ServeFile(HttpContext context, string filePath, string contentType)
{
    // Check if the file exists
    if (File.Exists(filePath))
    {
        // Set the response content type
        context.Response.ContentType = contentType;

        // Read the file contents
        var fileBytes = File.ReadAllText((
            Path.Combine(Directory.GetCurrentDirectory(), filePath
        )));

        return fileBytes;
        // // Write the file contents to the response stream
        // await context.Response.Body.WriteAsync(fileBytes, 0, fileBytes.Length);
    }
    else
    {
        // Return a not found response or handle the error as needed
        context.Response.StatusCode = StatusCodes.Status404NotFound;
        return "";
    }
}


// too lazy to make a file server
app.MapGet("/", (HttpContext context) => {
    return ServeFile(context, "wwwroot/index.html", "text/html");
});

app.MapGet("/script.js", (HttpContext context) => {
    return ServeFile(context, "wwwroot/script.js", "text/javascript");
});

app.MapGet("/font.ttf", (HttpContext context) => {
    return ServeFile(context, "wwwroot/font.ttf", "font/ttf");
});

WebSocketHandler ws = new("http://127.0.0.1:1337/");
GameClient gameClient = new(ws);

ws.SetGameClient(gameClient);

Thread serverThread = new Thread(() =>
{
    ws.Start().Wait();
});

serverThread.Start();
gameClient.JoinGame("127.0.0.1", 10052);
app.Run();
