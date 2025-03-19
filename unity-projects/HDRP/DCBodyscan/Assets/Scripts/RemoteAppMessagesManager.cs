
// system
using System;
using System.Collections.Generic;

// unity
using UnityEngine;

using BS;

// ############################## STATES

[Serializable]
public class HMDState {
    public string name = "DefaultVRHMD";
    public float battery = 0.75f;
    public Vector3 origin = Vector3.zero;
    public bool on = false;
}


[Serializable]
public class SessionState {
    public float elapsedTimeMs = 0f;
    public float totalTimeMs = 0f;
    public int currentChapterId = 0;
    public string currentStage = "None";
}
[Serializable]
public class BodyScanState {
    public int nbCamerasDefined = 0;
    public int nbCamerasConnected = 0;
    public int nbCamerasStreaming = 0;   
    public int currentCloudSize = 0;
}
[Serializable]
public class HapticDeviceState {
    bool isConnected = false;
}
[Serializable]
public class NetworkState {
    public string clientName = "DefaultPCName";
    public float upTimeMs = 0f;
}

[Serializable]
public class AHStates {
    public NetworkState network = new NetworkState();
    public SessionState session = new SessionState();
    public HMDState hmd = new HMDState();
    public BodyScanState bodyScan = new BodyScanState();
    public HapticDeviceState hapticDevice = new HapticDeviceState();
    // ...
}


// ############################## SETTINGS

[Serializable]
public class DebugSettings {
    public bool showUi = false;
}

[Serializable]
public class AudioSettings {
    public string device = "DefaultSoundDevice";
    public string language = "English";
    public float soundLevel = 0.5f;
}

[Serializable]
public class AHSettings {
    public DebugSettings debug = new DebugSettings();
    public AudioSettings audio = new AudioSettings();
    // hmd 
    // ...
}

// ############################## ACTION

[Serializable]
public class AHAction {
    public enum Type {
        SetStage
    }

    public int type = 0;
    public string value;
}

// ############################## CLIENT

    [Serializable]
public class AHClient {
    public string uuid = null;
    public string associatedServerUuid = null;
    public AHSettings settings = new AHSettings();
    public AHStates states = new AHStates();
}

// ############################## MESSAGES

[Serializable]
public class MessageFromClient {
    public string uuid = null;
    public string name = null;
    public int type;
    public string jsonContent = null;
}

[Serializable]
public class MessageToClient {
    public int type;
    //public string uuid = null; // not useful, the qml/C++ connection knows its uuid
}

[Serializable]
public class UpdateSettingsMessage : MessageToClient {
    public AHSettings settings = null;
}


[Serializable]
public class UpdateStatesMessage : MessageToClient {
    public AHStates states = null;
}

[Serializable]
public class ActionMessage : MessageToClient {
    public AHAction action = null;
}

[Serializable]
public class LogMessage : MessageToClient {
    public string log = null;
}



public class RemoteAppMessagesManager : MonoBehaviour{

    public enum MessageType {
        NewConnection= 0,
        UpdateSettings,
        UpdateStates,
        Action,
        CleanClients,
        Log,
        Error
    }

    public WebsocketServerComponent wss = null;


    public List<AHClient> clients = new List<AHClient>();
    public Dictionary<string, AHClient> clientPerServerConnectionUuid = new Dictionary<string, AHClient>();


    void Awake() {

        // init clients from json settings
        // ...

        if (wss != null) {
            wss.onOpen.AddListener((string connectionUuid) => {
                Debug.Log("New connection: " + connectionUuid);
                clientPerServerConnectionUuid.Add(connectionUuid, null);
            });
            wss.onClose.AddListener((string connectionUuid) => {

                Debug.Log("Connection closed " + connectionUuid +" " + clientPerServerConnectionUuid.ContainsKey(connectionUuid));
                if (clientPerServerConnectionUuid.ContainsKey(connectionUuid)) {
                    if (clientPerServerConnectionUuid[connectionUuid] != null) {
                        clientPerServerConnectionUuid[connectionUuid].associatedServerUuid = null;
                    }
                } else {
                    Debug.LogError(string.Format("Connection with id [{0}] removed, but it wasn't associated.", connectionUuid));
                }

                clientPerServerConnectionUuid.Remove(connectionUuid);
            });
            wss.onMessage.AddListener((string connectionUuid, string message) => {
                Debug.Log("Message received: " + message);
                if (clientPerServerConnectionUuid.ContainsKey(connectionUuid)) {
                    process_message(connectionUuid, message);
                } else {
                    Debug.LogError(string.Format("Invalid key from client [{0}], message not processed.", connectionUuid));
                }                
            });
        }
    }

    // Update is called once per frame
    void Update() {
        if (wss != null) {
            //dataJson = JsonUtility.ToJson(data);
            //wss.send_message_to_all_clients(dataJson);
            //nb++;
            foreach (var client in clients) {
                // define frequency
                if(client.associatedServerUuid != null) {
                    // send states
                    send_states_to_client(client);
                }
            }
        }
    }

    void send_settings_to_client(AHClient client) {
        UpdateSettingsMessage updateSettingsM = new UpdateSettingsMessage();
        updateSettingsM.type        = (int)MessageType.UpdateSettings;
        updateSettingsM.settings    = client.settings;
        wss.send_message_to_client(client.associatedServerUuid, JsonUtility.ToJson(updateSettingsM));
    }

    void send_states_to_client(AHClient client) {
        UpdateStatesMessage updateStatesM = new UpdateStatesMessage();
        updateStatesM.type = (int)MessageType.UpdateStates;
        updateStatesM.states = client.states;
        wss.send_message_to_client(client.associatedServerUuid, JsonUtility.ToJson(updateStatesM));
    }
    void send_log_message_to_client(AHClient client, string message) {
        LogMessage logM = new LogMessage();
        logM.type   = (int)MessageType.Log;
        logM.log    = message;
        wss.send_message_to_client(client.associatedServerUuid, JsonUtility.ToJson(logM));
    }

    void send_error_to_client(AHClient client, string error) {
        LogMessage logM = new LogMessage();
        logM.type   = (int)MessageType.Error;
        logM.log    = error;
        wss.send_message_to_client(client.associatedServerUuid, JsonUtility.ToJson(logM));
    }

    void process_new_connection(string connectionUuid, MessageFromClient cMessage) {

        if (cMessage.uuid == null) {
            Debug.LogError("Invalid [NewConnection] message, client name not defined.");
            return;
        }

        // look for existing clients 
        AHClient currentClient = null;
        foreach (var client in clients) {

            // check corresponding uuid
            if (cMessage.uuid == client.uuid) {                

                if (client.associatedServerUuid != null) {
                    Debug.LogError(string.Format("Client with anme [{0}] is already associated with id [{1}]", client.uuid, client.associatedServerUuid));
                    return;
                }

                currentClient = client;
                break;
            }
        }

        // if no client found, create a new one
        if (currentClient == null) {
            currentClient = new AHClient();
            currentClient.uuid = cMessage.uuid;
        }

        // associate client
        currentClient.associatedServerUuid = connectionUuid;
        clientPerServerConnectionUuid[connectionUuid] = currentClient;
        send_log_message_to_client(currentClient, "Client associated");

        // send settings to remote app
        send_settings_to_client(currentClient);
    }


    void process_message(string connectionUuid, string message) {

        try {
            var clientMessage = JsonUtility.FromJson<MessageFromClient>(message);
            switch ((MessageType)clientMessage.type) {
                case MessageType.NewConnection:
                    process_new_connection(connectionUuid, clientMessage);
                    break;
                case MessageType.CleanClients:

                    // ...

                    break;
                case MessageType.UpdateSettings: {
                    var nSettings = JsonUtility.FromJson<AHSettings>(clientMessage.jsonContent);
                    var client = clientPerServerConnectionUuid[connectionUuid];
                    client.settings = nSettings;
                    send_log_message_to_client(client, "Settings updated");
                }break;
                case MessageType.Action: {

                    //clientPerId[clientMessage.uuid];

                    var action = JsonUtility.FromJson<AHAction>(clientMessage.jsonContent);
                    var client = clientPerServerConnectionUuid[connectionUuid];

                    switch ((AHAction.Type)action.type) {
                        case AHAction.Type.SetStage:
                            // test
                            client.states.session.currentStage = action.value;
                            send_log_message_to_client(client, "Set stage action done");
                            break;
                    }
                }break;
            }
        } catch (Exception ex) {
            Debug.LogError(string.Format("Cannot process message: [{0}]", ex.Message));
        }


    }
}
