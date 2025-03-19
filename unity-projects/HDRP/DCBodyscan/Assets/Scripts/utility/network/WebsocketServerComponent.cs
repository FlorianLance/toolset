

/***********************************************************************************
** exvr-exp                                                                       **
** MIT License                                                                    **
** Copyright (c) [2024] [Florian Lance][Auxime]                                   **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
************************************************************************************/

// system
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Collections.Concurrent;

// unity
using UnityEngine;

using WebSocketServer;

namespace BS {

    public class TcpListenerJob : ThreadedJob {

        // parameters
        private bool m_useIpv6Address = false;
        private bool m_useAnyAddress = false;
        private string m_address;
        private int m_port;

        // states        
        private bool m_initialized = false;
        private volatile bool m_doLoop = false;
        private IPAddress m_finalIpAddress;

        // network
        public ConcurrentDictionary<string, WebSocketConnection> connnections = new ConcurrentDictionary<string, WebSocketConnection>();
        private TcpListener m_tcpListener = null;
        private WebSocketServer.WebSocketServer m_server = null;

        public int listening_port() {
            return m_port;
        }

        public string listening_address() {
            return m_address;
        }

        public IPAddress final_ip_address() {
            return m_finalIpAddress;
        }

        public TcpListenerJob(WebSocketServer.WebSocketServer server) {
            m_server = server;
        }

        public bool initialize(bool anyAddress, bool ipv6, string address, int port, bool exclusiveAddressUse) {

            m_useAnyAddress = anyAddress;
            m_useIpv6Address = ipv6;
            m_address = address;
            m_port = port;

            try {

                m_finalIpAddress = m_useIpv6Address ? IPAddress.IPv6Any : IPAddress.Any;

                if (!m_useAnyAddress) {

                    // parse for normal ip address
                    bool parsingSuccess = false;
                    IPAddress parsedAddress;
                    if (IPAddress.TryParse(address, out parsedAddress)) {
                        if (m_useIpv6Address && parsedAddress.AddressFamily == AddressFamily.InterNetworkV6) {
                            parsingSuccess = true;
                            m_finalIpAddress = parsedAddress;
                        } else if (!m_useIpv6Address && parsedAddress.AddressFamily == AddressFamily.InterNetwork) {
                            parsingSuccess = true;
                            m_finalIpAddress = parsedAddress;
                        }
                    }

                    if (!parsingSuccess) {

                        // try to get dns
                        bool dnsAddressFound = false;
                        IPAddress[] dnsAddresses = Dns.GetHostAddresses(address);
                        foreach (var dnsAddress in dnsAddresses) {

                            if (m_useIpv6Address && dnsAddress.AddressFamily == AddressFamily.InterNetworkV6) {
                                m_finalIpAddress = dnsAddress;
                                dnsAddressFound = true;
                                break;
                            } else if (!m_useIpv6Address && dnsAddress.AddressFamily == AddressFamily.InterNetwork) {
                                m_finalIpAddress = dnsAddress;
                                dnsAddressFound = true;
                                break;
                            }
                        }

                        if (!dnsAddressFound) {
                            m_finalIpAddress = m_useIpv6Address ? IPAddress.IPv6Any : IPAddress.Any;
                        }
                    }
                }

                m_tcpListener = new TcpListener(m_finalIpAddress, m_port);
                m_tcpListener.ExclusiveAddressUse = exclusiveAddressUse;
                m_tcpListener.Start();
                m_initialized = true;
            } catch (System.Net.Sockets.SocketException e) {
                Debug.LogError(string.Format("Socket error: {0}", e.Message));
                m_initialized = false;
            } catch (System.Exception e) {
                Debug.LogError(string.Format("Error: {0}", e.Message));
                m_initialized = false;
            }

            if (m_initialized) {
                // start thread
                m_doLoop = true;
                start();
            } else {
                m_tcpListener = null;
            }

            return m_initialized;
        }

        public void clean() {

            // stop thread 
            m_doLoop = false;

            if (!join(100)) {
                Debug.LogError(string.Format("Stop listener thread timeout."));
            }

            // remove connections
            foreach (var connection in connnections) {
                connection.Value.clean();
            }
            connnections = new ConcurrentDictionary<string, WebSocketConnection>();

            // stop listener
            if (m_tcpListener != null) {
                m_tcpListener.Stop();
            }

            m_tcpListener = null;
            m_server = null;
        }

        public void add_connection(WebSocketConnection connection) {
            Debug.Log(string.Format("Add connection {0}.", connection.uuid));
            connnections[connection.uuid] = connection;
        }
        public void remove_connection(WebSocketConnection connection) {
            Debug.Log(string.Format("Remove connection {0}.", connection.uuid));
            WebSocketConnection removedConnection = null;
            connnections.TryRemove(connection.uuid, out removedConnection);
            if (removedConnection != null) {
                Debug.Log(string.Format("Connection {0} succesfully removed.", removedConnection.uuid));
                removedConnection.clean();
            } else {
                Debug.LogError(string.Format("Cannot remove connection {0}.", connection.uuid));
            }
        }

        protected override void thread_function() {

            if (!m_initialized) {
                return;
            }

            try {

                while (m_doLoop) {

                    // check for waiting connection
                    while (!m_tcpListener.Pending()) {
                        System.Threading.Thread.Sleep(100);
                        if (!m_doLoop) {
                            return;
                        }
                    }

                    // accept a new client, then open a stream for reading and writing.
                    var connectedTcpClient = m_tcpListener.AcceptTcpClient();

                    // create a new connection
                    WebSocketConnection connection = new WebSocketConnection(connectedTcpClient, m_server);

                    // establish connection
                    connection.Establish();
                }
            } catch (SocketException socketException) {
                Debug.LogError(string.Format("Listening to connections, socketException {0}", socketException.ToString()));
                m_doLoop = false;
            }

            Debug.Log("Stop listening for connections.");
        }
    }

    public static class XLExtensions {
        public static IEnumerable<string> split_in_groups(this string original, int size) {
            var p = 0;
            var l = original.Length;
            while (l - p > size) {
                yield return original.Substring(p, size);
                p += size;
            }
            yield return original.Substring(p);
        }
    }

    public class MyWebSocketServer : WebSocketServer.WebSocketServer {

        TcpListenerJob m_listener = null;

        public bool initialize(bool anyAddress, bool ipv6, string address, int port, bool exclusiveAddressUse) {
            m_listener = new TcpListenerJob(this);
            if (m_listener.initialize(anyAddress, ipv6, address, port, exclusiveAddressUse)) {
                return true;
            } else {
                m_listener = null;
                return false;
            }
        }

        public void clean() {
            if (m_listener != null) {
                m_listener.clean();
                m_listener = null;
            }
        }

        public IPAddress get_listening_ip_address() {
            return m_listener.final_ip_address();
        }

        public WebSocketOpenEvent op_open_event() {
            return onOpen;
        }

        public WebSocketCloseEvent op_close_event() {
            return onClose;
        }

        public WebSocketMessageEvent op_message_event() {
            return onMessage;
        }

        override public void on_open(WebSocketConnection connection) {
            m_listener.add_connection(connection);
        }

        override public void on_message(WebSocketMessage message) {
            Debug.Log(string.Format("Message received from client [{0}] : [{1}]", message.connection.uuid, message.data));
        }

        override public void on_close(WebSocketConnection connection) {
            m_listener.remove_connection(connection);
        }

        public void send_message_to_client(string connectionUuid, string message) {
            if (m_listener.connnections.ContainsKey(connectionUuid)) {
                m_listener.connnections[connectionUuid].send_message(message.split_in_groups(125));
            }
        }
        public void send_messages_to_all_clients(string message) {
            var splittedMessage = message.split_in_groups(125);
            foreach (var connection in m_listener.connnections) {
                connection.Value.send_message(splittedMessage);
            }
        }
    }

    public class WebsocketServerComponent : MonoBehaviour {

        [System.Serializable]
        public class OpenConnectionEvent : UnityEngine.Events.UnityEvent<string> { }

        [System.Serializable]
        public class NewMessageEvent : UnityEngine.Events.UnityEvent<string, string> { }

        [System.Serializable]
        public class CloseConnectionEvent : UnityEngine.Events.UnityEvent<string> { }


        public bool initializeAtStart = true;

        public bool anyAddress = true;
        public string address = "127.0.0.1";
        public bool useIpv6 = false;
        public int port = 8080;
        public bool exclusiveAddressUse = false;

        public OpenConnectionEvent onOpen = new OpenConnectionEvent();
        public NewMessageEvent onMessage = new NewMessageEvent();
        public CloseConnectionEvent onClose = new CloseConnectionEvent();

        private MyWebSocketServer m_server = null;

        public bool initialize() {

            // create server
            m_server = new MyWebSocketServer();

            // set signals
            m_server.onMessage.AddListener((WebSocketMessage message) => {                
                onMessage.Invoke(message.connection.uuid, message.data);
            });
            m_server.onOpen.AddListener((WebSocketConnection connection) => {
                onOpen.Invoke(connection.uuid);
            });
            m_server.onClose.AddListener((WebSocketConnection connection) => {
                onClose.Invoke(connection.uuid);
            });

            // initialize
            if (m_server.initialize(anyAddress, useIpv6, address, port, exclusiveAddressUse)) {
                return true;
            }
            m_server = null;

            return false;
        }

        public void send_message_to_client(string connectionUuid, string message) {
            if (m_server != null) {
                m_server.send_message_to_client(connectionUuid, message);
            }
        }

        public void send_message_to_all_clients(string message) {
            if (m_server != null) {
                m_server.send_messages_to_all_clients(message);
            }
        }


        void Start() {
            if (initializeAtStart) {
                initialize();
            }
        }

        void Update() {
            // always update
            if (m_server != null) {
                m_server.process_events();
            }
        }

        private void OnApplicationQuit() {
            if (m_server != null) {
                m_server.clean();
                m_server = null;
            }
        }
    }

}


