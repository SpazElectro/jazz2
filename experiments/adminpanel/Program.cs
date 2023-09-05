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


app.MapGet("/", (HttpContext context) =>
{
    string s = ServeFile(context, "wwwroot/index.html", "text/html");

    return s;
});

WebSocketHandler ws = new("http://127.0.0.1:1337/");
Thread serverThread = new Thread(() =>
{
    ws.Start().Wait();
});

serverThread.Start();

// GameClient.joinGame("127.0.0.1", 10052);

app.Run();
