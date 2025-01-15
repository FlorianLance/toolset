

using System;
using System.Net;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
using System.Text;

using UnityEngine;
using NetCoreServer;

class TestSession : WssSession {
    public TestSession(WssServer server) : base(server) { }

    public override void OnWsConnected(HttpRequest request) {
        //Console.WriteLine($"Chat WebSocket session with Id {Id} connected!");

        // Send invite message
        string message = "Hello from WebSocket chat! Please send a message or '!' to disconnect the client!";
        SendTextAsync(message);
    }

    public override void OnWsDisconnected() {
        //Console.WriteLine($"Chat WebSocket session with Id {Id} disconnected!");
    }

    public override void OnWsReceived(byte[] buffer, long offset, long size) {
        string message = Encoding.UTF8.GetString(buffer, (int)offset, (int)size);
        //Console.WriteLine("Incoming: " + message);

        // Multicast message to all connected sessions
        ((WssServer)Server).MulticastText(message);

        // If the buffer starts with '!' the disconnect the current session
        if (message == "!") {
            //Close();
        }
    }

    protected override void OnError(SocketError error) {
        Console.WriteLine($"Chat WebSocket session caught an error with code {error}");
    }
}
public class TestServer : WssServer {


    public TestServer(SslContext context, IPAddress address, int port) : base(context, address, port) { }

    protected override SslSession CreateSession() { return new TestSession(this); }

    protected override void OnError(SocketError error) {
        Console.WriteLine($"Chat WebSocket server caught an error with code {error}");
    }
}

public class WebsocketServer : MonoBehaviour {

    TestServer server = null;

    protected void Awake() {

        int port = 8443;

        var context = new SslContext(SslProtocols.Tls13, new X509Certificate2("server.pfx", "qwerty"));


        server = new TestServer(context, IPAddress.Any);
    }
}