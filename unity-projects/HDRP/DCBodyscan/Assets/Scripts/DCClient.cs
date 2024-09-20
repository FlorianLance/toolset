
/***********************************************************************************
** MIT License                                                                    **
** Copyright (c) [2024] [Florian Lance]                                           **
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
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Generic;

// unity
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif
using UnityEngine.Profiling;
using UnityEngine.VFX;
using UnityEngine.Events;

namespace BS {


#if UNITY_EDITOR

    [CustomEditor(typeof(DCClient))]
    public class DCNetworkDirectPlayerEditorVFX : Editor {

        private DCClient dcNetworkDIrectPlayer { get { return target as DCClient; } }

        public override bool RequiresConstantRepaint() {
            return true;
        }

        public override void OnInspectorGUI() {

            base.OnInspectorGUI();

            var dcNDP = dcNetworkDIrectPlayer;
            var origFontStyle = EditorStyles.label.fontStyle;

            EditorGUILayout.Separator();

            EditorStyles.label.fontStyle = FontStyle.Bold;
            EditorGUILayout.LabelField("#### ACTIONS ####");
            EditorStyles.label.fontStyle = origFontStyle;

            EditorGUI.BeginDisabledGroup(!Application.isPlaying);

            EditorGUILayout.BeginHorizontal();
            if (GUILayout.Button("Connect all")) {
                dcNDP.connect_to_devices();
            }
            if (GUILayout.Button("Disconnect all")) {
                dcNDP.disconnect_from_devices();
            }
            EditorGUILayout.EndHorizontal();

            if (GUILayout.Button("Apply device settings file")) {
                dcNDP.apply_device_settings();
            }
            if (GUILayout.Button("Apply filters settings file")) {
                dcNDP.apply_filters_settings();
            }
            if (GUILayout.Button("Apply color settings file")) {
                dcNDP.apply_color_settings();
            }

            EditorGUI.EndDisabledGroup();
        }
    }
#endif

    public class DCClient : MonoBehaviour {

        [Header("###### ACTIONS TO DO######")]
        public bool initAtAwake = true;
        public bool connectAtStart = true;
        public bool readProcessAtStart = true;        
        public bool applyDeviceSettingsAfterConnection = true;       
        public bool applyFiltersSettingsAfterConnection = false;
        public bool applyColorSettingsAfterConnection = false;
        public bool loadDisplaySettingsFilesAfterInitializing = false;
        public string displaySettingsFilesBasePath = "./display_settings";

        [Header("###### SETTINGS FILES PATHS ######")]
        public string clientSettingsFilePath = "";

        [Header("#### STATES ####")]
        [SerializeField]
        private bool m_allDevicesConnected = false;
        [SerializeField]
        private bool m_threadsStarted = false;
        [SerializeField]
        private List<DCStreamingStates> devicesStates = new List<DCStreamingStates>();

        [Header("#### EVENTS ####")]
        public UnityEvent allClientsConnectedEvent = new UnityEvent();
        public UnityEvent streamingStartedEvent = new UnityEvent();
        public UnityEvent streamingEndedEvent = new UnityEvent();

        private DCClientDLL m_dcClientDLL = null;
        private DCDataVFX m_dcDataVFX = null;

        // jobs
        private List<DCReadNetworkDataJob> m_readNetworkDataJobs = new List<DCReadNetworkDataJob>();
        private List<DCTriggerReceivedPacketsJob> m_triggerReceievedPacketsJobs = new List<DCTriggerReceivedPacketsJob>();       
        private List<DCProcessDataJob> m_processDataJobs = new List<DCProcessDataJob>();

        // callbacks
        private LogMessageCB logMessageCB = null;
        private NewFeedbackCB newFeedbackCB = null;


        #region public_functions

        public void initialize() {

            clean();

            // init dll
            m_dcClientDLL = new DCClientDLL();
            m_dcClientDLL.init_callbacks(logMessageCB, newFeedbackCB);
            if (m_dcClientDLL.initialize(clientSettingsFilePath, false)) {
                initialize_data();
            }            
        }

        public void clean() {

            if (m_dcClientDLL != null) {

                // stop reading
                if (m_threadsStarted) {
                    stop_threads();
                }

                // clean jobs
                m_readNetworkDataJobs = null;
                m_triggerReceievedPacketsJobs = null;
                m_processDataJobs = null;

                // disconnect devices
                disconnect_from_devices();

                // clean DLL
                m_dcClientDLL.dispose();
                m_dcClientDLL = null;
            }

            m_allDevicesConnected = false;
            m_threadsStarted = false;
        }

        public void start_threads() {

            if (m_dcClientDLL != null) {

                //foreach (var deviceS in devicesStates) {
                //    deviceS.cloudUpdated = 0;
                //    deviceS.averageCloudUpdatedPerSecond = 0;
                //    deviceS.sw.Start();
                //}

                foreach (var rndJob in m_readNetworkDataJobs) {
                    rndJob.start_reading();
                }

                foreach (var trJob in m_triggerReceievedPacketsJobs) {
                    trJob.start_triggering();
                }

                foreach (var pdJob in m_processDataJobs) {
                    pdJob.start_processing();
                }

                m_threadsStarted = true;
                streamingStartedEvent.Invoke();
            }
        }

        public void stop_threads() {

            if (m_dcClientDLL != null) {
                m_threadsStarted = false;

                foreach (var rndJob in m_readNetworkDataJobs) {
                    rndJob.stop_reading();
                }

                foreach (var trJob in m_triggerReceievedPacketsJobs) {
                    trJob.stop_triggering();
                }                

                foreach (var pdJob in m_processDataJobs) {
                    pdJob.stop_processing();
                }

                //foreach (var deviceS in devicesStates) {
                //    deviceS.sw.Stop();
                //}
                streamingEndedEvent.Invoke();
            }
        }

        // actions
        public void connect_to_devices() {
            if (m_dcClientDLL != null) {
                m_dcClientDLL.connect_to_devices();
            }
        }
        public void disconnect_from_devices() {
            if (m_dcClientDLL != null) {
                m_dcClientDLL.disconnect_from_devices();
            }
        }

        // states
        public int devices_nb() {

            if (m_dcClientDLL != null) {
                return m_dcClientDLL.devices_nb();
            }
            return 0;
        }

        public int connected_devices_nb() {
            int countConnected = 0;
            for (int idD = 0; idD < devices_nb(); ++idD) {
                if (is_device_connected(idD)) {
                    ++countConnected;
                }
            }
            return countConnected;
        }
        public bool is_device_connected(int idD) {
            if (m_dcClientDLL != null) {
                return m_dcClientDLL.is_device_connected(idD);
            }
            return false;
        }
        public int current_frame_id(int idD) {
            if (m_dcClientDLL != null) {
                return m_dcClientDLL.current_frame_id(idD);
            }
            return 0;
        }
        public int current_frame_cloud_size(int idD) {
            if (m_dcClientDLL != null) {
                return m_dcClientDLL.current_frame_cloud_size(idD);
            }
            return 0;
        }

        public DCClientDLL get_dll() {
            return m_dcClientDLL;
        }

        // settings
        public void apply_device_settings() {
            if (m_dcClientDLL != null) {
                for (int idD = 0; idD < m_dcClientDLL.devices_nb(); ++idD) {                    
                    UnityEngine.Debug.Log(string.Format("[DClient::apply_device_settings] Wait device [{0}] to be updated. ", idD));
                    Thread.Sleep(250);
                    m_dcClientDLL.apply_devices_settings(idD);      
                }
            }
        }
        public void apply_color_settings() {
            if (m_dcClientDLL != null) {
                m_dcClientDLL.apply_color_settings();
            }
        }
        public void apply_filters_settings() {
            if (m_dcClientDLL != null) {
                m_dcClientDLL.apply_filters_settings();
            }
        }

        #endregion

        #region private_functions

        public void initialize_data() {

            int nbDevices = devices_nb();
            m_dcDataVFX.initialize(nbDevices);
            devicesStates = new List<DCStreamingStates>(nbDevices);

            for (int idC = 0; idC < nbDevices; ++idC) {
                var devicestates = new DCStreamingStates();
                devicestates.deviceId = idC;
                devicesStates.Add(devicestates);
            }

            if (loadDisplaySettingsFilesAfterInitializing) {
                for (int idG = 0; idG < m_dcDataVFX.displaySettings.Count; ++idG) {
                    string path = string.Format("{0}_{1}.json", displaySettingsFilesBasePath, idG);
                    UnityEngine.Debug.Log(string.Format("[DClient::initialize_data] Load display settings file with path [{0}]. ", path));
                    m_dcDataVFX.update_display_parameters_from_json_file(idG, path);
                }
            }

            create_jobs(nbDevices);

            // if devices  are local, set them as connected
            for (int idC = 0; idC < nbDevices; ++idC) {
                if (m_dcClientDLL.is_local(idC)) {
                    devicesStates[idC].connected = true;
                }
            }

        }

        private void create_jobs(int nbDevices) {

            // read network data
            m_readNetworkDataJobs = new List<DCReadNetworkDataJob>();
            for (int idC = 0; idC < nbDevices; ++idC) {
                var rndJob = new DCReadNetworkDataJob();
                rndJob.dllPlayer = m_dcClientDLL;
                rndJob.idD = idC;
                m_readNetworkDataJobs.Add(rndJob);
            }

            // trigger received packets
            m_triggerReceievedPacketsJobs = new List<DCTriggerReceivedPacketsJob>();
            for (int idC = 0; idC < nbDevices; ++idC) {
                var trJob = new DCTriggerReceivedPacketsJob();
                trJob.dllPlayer = m_dcClientDLL;
                trJob.idD = idC;
                m_triggerReceievedPacketsJobs.Add(trJob);
            }
            // process data
            m_processDataJobs = new List<DCProcessDataJob>();
            for (int idC = 0; idC < nbDevices; ++idC) {
                var pdJob = new DCProcessDataJob();
                pdJob.dllPlayer = m_dcClientDLL;
                pdJob.idD = idC;
                m_processDataJobs.Add(pdJob);
            }
        }
        private void new_feedback(int idC, int messageType, int feedbackType) {

            string messageTypeStr;
            switch (messageType) {
                case 0:
                    messageTypeStr = "init_network_infos";
                    break;
                case 1:
                    messageTypeStr = "update_device_settings";
                    break;
                case 2:
                    messageTypeStr = "update_color_settings";
                    break;
                case 3:
                    messageTypeStr = "update_filters";
                    break;
                case 4:
                    messageTypeStr = "compressed_frame_data";
                    break;
                case 5:
                    messageTypeStr = "command";
                    break;
                case 6:
                    messageTypeStr = "feedback";
                    break;
                case 7:
                    messageTypeStr = "delay";
                    break;
                case 8:
                    messageTypeStr = "synchro";
                    break;
                case 9:
                    messageTypeStr = "ping";
                    break;
                default:
                    messageTypeStr = "invalid";
                    break;
            }

            string feedbackTypeStr;
            switch (feedbackType) {
                case 0:
                    feedbackTypeStr = "message_received";
                    break;
                case 1:
                    feedbackTypeStr = "timeout";
                    break;
                case 2:
                    feedbackTypeStr = "disconnect";
                    break;
                case 3:
                    feedbackTypeStr = "quit";
                    break;
                case 4:
                    feedbackTypeStr = "shutdown";
                    break;
                case 5:
                    feedbackTypeStr = "restart";
                    break;
                default:
                    feedbackTypeStr = "invalid";
                    break;
            }

            UnityEngine.Debug.Log(string.Format("[DClient::new_feedback] Receive message of type [{0}] from device [{1}] with feeback [{2}] from Thread [{3}]", messageTypeStr, idC, feedbackTypeStr, Thread.CurrentThread.Name));

            if (messageType == 0 && feedbackType == 0) {
                devicesStates[idC].connected = true;
            }

            if (messageType == 6 && feedbackType == 2) {
                devicesStates[idC].connected = false;
                m_allDevicesConnected = false;
            }

            check_devices_connected_state();
        }

        private void check_devices_connected_state() {

            if (!m_allDevicesConnected) {
                bool allConnected = true;
                foreach (var deviceState in devicesStates) {
                    if (!deviceState.connected) {
                        allConnected = false;
                        break;
                    }
                }
                m_allDevicesConnected = allConnected;

                if (m_allDevicesConnected) {

                    allClientsConnectedEvent.Invoke();

                    UnityEngine.Debug.Log(string.Format("[DClient::check_devices_connected_state] All devices are connected [{0}].", devicesStates.Count));
                    if (applyDeviceSettingsAfterConnection) {
                        apply_device_settings();
                    }
                    if (applyFiltersSettingsAfterConnection) {
                        apply_filters_settings();
                    }
                    if (applyColorSettingsAfterConnection) {
                        apply_color_settings();
                    }
                }
            }
        }

        #endregion

        #region mono_behaviour_functions

        private void Awake() {

            // init callbacks
            if (logMessageCB == null) {
                logMessageCB = (string message, int type) => {
                    if (type == 0) {
                        UnityEngine.Debug.Log(message);
                    } else if (type == 1) {
                        UnityEngine.Debug.LogWarning(message);
                    } else if (type == 2) {
                        UnityEngine.Debug.LogError(message);
                    }
                };
            }

            newFeedbackCB = (int idC, int messageType, int feedbackType) => {
                new_feedback(idC, messageType, feedbackType);
            };

            m_dcDataVFX = GetComponent<DCDataVFX>();
            if (m_dcDataVFX == null) {
                m_dcDataVFX = gameObject.AddComponent<DCDataVFX>();
            }

            if (m_dcDataVFX.sharedVisualEffectAsset == null) {
                m_dcDataVFX.sharedVisualEffectAsset = Resources.Load<VisualEffectAsset>("VisualEffectAssets/DCCloud");
            }

            if (initAtAwake) {
                initialize();
            }
        }


        private void Start() {

            if (readProcessAtStart) {
                start_threads();
            }

            if (connectAtStart) {
                connect_to_devices();
            }

            check_devices_connected_state();
        }

        private void Update() {

            if (m_dcClientDLL == null) {
                return;
            }

            // update shaders parameters
            foreach (var deviceStates in devicesStates) {
                m_dcDataVFX.update_shader_from_display_parameters(deviceStates.deviceId);
            }

            if (!m_threadsStarted) {
                return;
            }

            Profiler.BeginSample("[DClient::update] Update DLL");
            m_dcClientDLL.update();
            Profiler.EndSample();

            Profiler.BeginSample("[DClient::update] uncompress_frame / copy_current_frame_vertices_vfx");
            Parallel.ForEach(devicesStates, deviceStates => {

                if (m_dcClientDLL.is_frame_available(deviceStates.deviceId)) {

                    deviceStates.currentFrameSize = m_dcClientDLL.current_frame_cloud_size(deviceStates.deviceId);
                    deviceStates.currentId = m_dcClientDLL.current_frame_id(deviceStates.deviceId);

                    deviceStates.verticesCountToCopy = m_dcClientDLL.copy_current_frame_vertices_vfx(
                        deviceStates.deviceId,
                        m_dcDataVFX.positions_native_buffer(deviceStates.deviceId),
                        m_dcDataVFX.colors_native_buffer(deviceStates.deviceId),
                        m_dcDataVFX.normals_native_buffer(deviceStates.deviceId),
                        deviceStates.currentFrameSize,
                        true
                    );

                    m_dcClientDLL.invalidate_frame(deviceStates.deviceId);
                } else {
                    deviceStates.verticesCountToCopy = -1;
                }

                //if (m_dllPlayer.retrieve_last_frame(deviceStates.deviceId)) {

                //    deviceStates.currentFrameSize = m_dllPlayer.current_frame_cloud_size(deviceStates.deviceId);
                //    deviceStates.currentId = m_dllPlayer.current_frame_id(deviceStates.deviceId);

                //    deviceStates.verticesCountToCopy = m_dllPlayer.copy_current_frame_vertices_vfx(
                //        deviceStates.deviceId,
                //        dataVFX.positions_native_buffer(deviceStates.deviceId),
                //        dataVFX.colors_native_buffer(deviceStates.deviceId),
                //        dataVFX.normals_native_buffer(deviceStates.deviceId),
                //        deviceStates.currentFrameSize,
                //        true
                //    );
                //} else {
                //    //deviceStates.verticesCountToCopy = -1;
                //}
            });
            Profiler.EndSample();

            Profiler.BeginSample("[DClient::update] Update VFX data");
            foreach (var deviceStates in devicesStates) {
                m_dcDataVFX.update_data(deviceStates.deviceId, deviceStates.verticesCountToCopy);
            }
            Profiler.EndSample();

        }

        private void OnApplicationQuit() {
            clean();
        }

        #endregion
    }
}