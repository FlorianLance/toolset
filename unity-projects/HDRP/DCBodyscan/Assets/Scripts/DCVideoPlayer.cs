

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
using System.Collections.Generic;
using System.Threading.Tasks;

// unity
#if UNITY_EDITOR
using UnityEditor;
#endif
using UnityEngine;
using UnityEngine.VFX;
using UnityEngine.Profiling;
using UnityEngine.Events;


namespace BS {


#if UNITY_EDITOR

    [CustomEditor(typeof(DCVideoPlayer))]
    public class DCVideoPlayerVFXEditor : Editor {

        private DCVideoPlayer dcVideoPlayer { get { return target as DCVideoPlayer; } }

        public override bool RequiresConstantRepaint() {
            return true;
        }

        public override void OnInspectorGUI() {

            var origFontStyle = EditorStyles.label.fontStyle;
            EditorStyles.label.fontStyle = FontStyle.Bold;
            EditorGUILayout.LabelField("#### ACTIONS ####");
            EditorStyles.label.fontStyle = origFontStyle;

            if (dcVideoPlayer.is_loaded()) {
                if (dcVideoPlayer.is_started()) {
                    if (dcVideoPlayer.is_playing()) {
                        EditorGUILayout.LabelField("Video is started and playing.");
                    } else {
                        EditorGUILayout.LabelField("Video is started and pausing.");
                    }
                } else {
                    EditorGUILayout.LabelField("Video is loaded and not started.");
                }
            } else {
                EditorGUILayout.LabelField("Video not loaded.");
            }

            EditorGUI.BeginDisabledGroup(!Application.isPlaying);

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Video file path: ");
            dcVideoPlayer.videoFilePath = EditorGUILayout.TextField(dcVideoPlayer.videoFilePath);
            if (GUILayout.Button("Load video")) {
                dcVideoPlayer.load_video_from_file(dcVideoPlayer.videoFilePath);
            }
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            if (!dcVideoPlayer.is_started()) {
                if (GUILayout.Button("Start video")) {
                    dcVideoPlayer.start_video();
                }
            } else {
                if (GUILayout.Button("Stop video")) {
                    dcVideoPlayer.stop_video();
                }
                if (dcVideoPlayer.is_playing()) {
                    if (GUILayout.Button("Pause")) {
                        dcVideoPlayer.pause_video();
                    }
                } else {
                    if (GUILayout.Button("Unpause")) {
                        dcVideoPlayer.unpause_video();
                    }
                }
            }
            EditorGUILayout.EndHorizontal();

            EditorGUILayout.BeginHorizontal();
            if (GUILayout.Button("Go to start time")) {
                dcVideoPlayer.go_to_start_time();
                dcVideoPlayer.update();
            }
            if (GUILayout.Button("Go to end time")) {
                dcVideoPlayer.go_to_end_time();
                dcVideoPlayer.update();
            }
            EditorGUILayout.EndHorizontal();

            bool videoLoaded = dcVideoPlayer.is_loaded();

            float durationMs = videoLoaded ? dcVideoPlayer.duration_ms() : 0f;
            float currentTimeMs = videoLoaded ? dcVideoPlayer.current_time_ms() : 0f;
            EditorGUILayout.LabelField("Video duration: " + durationMs + "ms");

            if (!EditorApplication.isPaused) {

                EditorGUILayout.BeginHorizontal();
                EditorGUILayout.LabelField("Current time: " + currentTimeMs + "ms");

                float currentFactor = currentTimeMs / durationMs;
                float newFactor = EditorGUILayout.Slider(currentFactor, 0f, 1f);
                if (newFactor != currentFactor) {
                    dcVideoPlayer.set_current_time(newFactor * durationMs);
                    dcVideoPlayer.update();
                }
                EditorGUILayout.EndHorizontal();
            }

            EditorGUI.EndDisabledGroup();

            EditorGUILayout.Separator();
            base.OnInspectorGUI();
        }
    }
#endif

    public class DCVideoPlayer : MonoBehaviour {

        public enum ActionWhenVideoEnds {
            DoLoop,
            DoPause,
            DoStop
        }

        [Header("#### SETTINGS ####")]
        public string videoFilePath = "";
        public bool loadAtStart = false;
        public bool playWhenLoaded = false;

        [Header("#### DISPLAY ####")]
        public bool loadDisplaySettingsFilesAfterVideoLoading = false;
        public string displaySettingsFilesBasePath = "./display_settings";
        public VisualEffectAsset visualEffectAsset = null;

        [Header("#### AUDIO ####")]
        public AudioSource externalAudioSource = null;
        public float audioTimeOffsetS = 0f;

        [Header("#### NAVIGATION ####")]
        public bool startVideoAtSpecificTime = false;
        public float startTimeMS = 0f;
        public bool endVideoAtSpecificTime = false;
        public float endTimeMS = 10000f;
        public ActionWhenVideoEnds endAction = ActionWhenVideoEnds.DoLoop;

        [Header("#### STATES ####")]
        [SerializeField]
        private volatile bool m_videoLoaded = false;
        private volatile bool m_isLoading = false;
        [SerializeField]
        private List<DCVideoStates> devicesStates = new List<DCVideoStates>();

        private DCVideoPlayerDLL m_dllPlayer = null;
        private DCDataVFX dataVFX = null;

        [Header("#### EVENTS ####")]
        public UnityEvent videoLoadedEvent = new UnityEvent();
        public UnityEvent videoStartedEvent = new UnityEvent();
        public UnityEvent videoStoppedEvent = new UnityEvent();
        public UnityEvent videoPausedEvent = new UnityEvent();
        public UnityEvent videoUnpausedEvent = new UnityEvent();
        public UnityEvent videoEndReachedEvent = new UnityEvent();
        public UnityEvent<float, float> videoTimes = new UnityEvent<float, float>();

        // jobs
        ActionSignalJob<string> loadVideoJob = null;

        #region private_functions

        private void update_settings() {
            set_player_settings(endAction == ActionWhenVideoEnds.DoLoop, startVideoAtSpecificTime ? startTimeMS : -1f, endVideoAtSpecificTime ? endTimeMS : -1f);
        }

        private void initialize_data_from_video(bool success) {
            
            m_isLoading   = false;
            m_videoLoaded = success;
            if (m_videoLoaded) {

                int nbCameras = m_dllPlayer.cameras_count();
                dataVFX.initialize(nbCameras);
                devicesStates = new List<DCVideoStates>(nbCameras);

                for (int idC = 0; idC < nbCameras; ++idC) {
                    var devicestates = new DCVideoStates();
                    devicestates.deviceId = idC;
                    m_dllPlayer.copy_transform(idC, devicestates.deviceTransform);
                    devicesStates.Add(devicestates);
                }

                m_videoLoaded = true;
                if (loadDisplaySettingsFilesAfterVideoLoading) {
                    for (int idG = 0; idG < dataVFX.displaySettings.Count; ++idG) {
                        string path = string.Format("{0}_{1}.json", displaySettingsFilesBasePath, idG);
                        UnityEngine.Debug.Log(string.Format("[DCVideoPlayerVFX::load_video_from_file] Load display settings file with path [{0}]. ", path));
                        dataVFX.update_display_parameters_from_json_file(idG, path);
                    }
                }
                videoLoadedEvent.Invoke();

                if (is_loaded() && playWhenLoaded) {
                    start_video();
                }
            }
        }

        #endregion


        #region public_functions

        public void load_video_from_file(string videoFilePath) {

            if (m_isLoading) {
                return;
            }

            // reset states
            m_isLoading     = true;
            m_videoLoaded   = false;

            if (!ExecuteFromMainThread.available) {
                gameObject.AddComponent<ExecuteFromMainThread>();
            }

            // start loading video job
            loadVideoJob.parameter                 = System.IO.Path.Combine(Application.dataPath, videoFilePath);
            loadVideoJob.function                  = m_dllPlayer.load_from_file;
            loadVideoJob.signal                    = initialize_data_from_video;
            loadVideoJob.triggerSignalOnMainThread = true;
            loadVideoJob.start();            
        }

        public bool is_loaded() {
            if (m_dllPlayer != null) {
                return m_videoLoaded;
            }
            return false;
        }
        public bool is_started() {
            if (is_loaded()) {
                return m_dllPlayer.is_started();
            }
            return false;
        }
        public bool is_playing() {
            if (is_loaded()) {
                return m_dllPlayer.is_playing();
            }
            return false;
        }
        public bool is_looping() {
            if (is_loaded()) {
                return m_dllPlayer.is_looping();
            }
            return false;
        }
        public float duration_ms() {
            if (is_loaded()) {
                return m_dllPlayer.duration_ms();
            }
            return 0f;
        }
        public float current_time_ms() {
            if (is_loaded()) {
                return m_dllPlayer.current_time_ms();
            }
            return 0f;
        }
        public float start_time_ms() {
            if (is_loaded()) {
                return m_dllPlayer.start_time_ms();
            }
            return -1f;
        }
        public float end_time_ms() {
            if (is_loaded()) {
                return m_dllPlayer.end_time_ms();
            }
            return -1f;
        }

        public void set_player_settings(bool doLoops, float startTimeMs = -1f, float endTimeMs = -1f) {
            m_dllPlayer.set_player_settings(doLoops, startTimeMs, endTimeMs);
        }

        public void start_video() {
            if (is_loaded()) {
                if (!is_started()) {
                    update_settings();
                    m_dllPlayer.start_video();
                    if(externalAudioSource != null) {                        
                        externalAudioSource.time = audioTimeOffsetS * 0.001f;
                        externalAudioSource.Play();
                    }
                    videoStartedEvent.Invoke();
                } else {
                    unpause_video();
                }
            }
        }

        public void stop_video() {
            if (is_loaded()) {
                update_settings();
                m_dllPlayer.stop_video();
                if (externalAudioSource != null) {
                    externalAudioSource.Stop();
                }
                videoStoppedEvent.Invoke();
            }
        }

        public void pause_video() {
            if (is_loaded()) {
                update_settings();
                m_dllPlayer.pause_video();
                if (externalAudioSource != null) {
                    externalAudioSource.Pause();
                }
                videoPausedEvent.Invoke();
            }
        }
        public void unpause_video() {
            if (is_loaded()) {
                update_settings();
                m_dllPlayer.unpause_video();
                if (externalAudioSource != null) {
                    externalAudioSource.UnPause();
                }
                videoUnpausedEvent.Invoke();
            }
        }

        public void go_to_start_time() {

            if (is_loaded()) {
                update_settings();
                m_dllPlayer.go_to_start_time();
                if (externalAudioSource != null) {
                    externalAudioSource.time = audioTimeOffsetS * 0.001f;
                }
                
            }
        }

        public void go_to_end_time() {

            if (is_loaded()) {
                update_settings();
                m_dllPlayer.go_to_end_time();
                if (externalAudioSource != null) {                    
                    externalAudioSource.time = m_dllPlayer.current_time_ms() + audioTimeOffsetS * 0.001f;
                }
            }
        }

        public void set_current_time(float timeMs) {
            if (is_loaded()) {
                update_settings();
                m_dllPlayer.set_current_time(timeMs);
                if (externalAudioSource != null) {
                    externalAudioSource.time = m_dllPlayer.current_time_ms() + audioTimeOffsetS * 0.001f;
                }
            }
        }

        public void update() {


            if (!is_loaded()) {
                return;
            }

            update_settings();

            var currentTimeMS = m_dllPlayer.current_time_ms();
            bool videoEndReached = false;
            if (endVideoAtSpecificTime) {
                if (currentTimeMS > endTimeMS) {
                    videoEndReached = true;
                }
            } else {
                if (currentTimeMS > duration_ms()) {
                    videoEndReached = true;
                }
            }

            videoTimes.Invoke(currentTimeMS - start_time_ms(), end_time_ms() - currentTimeMS);

            // store current frames id before updating
            for (int idC = 0; idC < m_dllPlayer.cameras_count(); idC++) {
                devicesStates[idC].currentFrameId = m_dllPlayer.current_frame_id(idC);
            }

            Profiler.BeginSample("[DCVideoPlayerVFX::update] Update DLL player");
            m_dllPlayer.update();
            Profiler.EndSample();

            Profiler.BeginSample("[DCVideoPlayerVFX::update] Copy data from DLL player");
            Parallel.ForEach(devicesStates, deviceStates => {

                int currentId = m_dllPlayer.current_frame_id(deviceStates.deviceId);

                if (deviceStates.currentFrameId != currentId) {
                    deviceStates.currentFrameId = currentId;
                    deviceStates.currentFrameSize = m_dllPlayer.get_current_frame_valid_vertices_count(deviceStates.deviceId);
                    
                    deviceStates.verticesCountToCopy = m_dllPlayer.copy_camera_cloud_vfx(
                        deviceStates.deviceId,
                        dataVFX.positions_native_buffer(deviceStates.deviceId),
                        dataVFX.colors_native_buffer(deviceStates.deviceId),
                        dataVFX.normals_native_buffer(deviceStates.deviceId),
                        deviceStates.currentFrameSize,
                        true
                    );

                } else {
                    deviceStates.verticesCountToCopy = -1;
                }
            });
            Profiler.EndSample();

            Profiler.BeginSample("[DCVideoPlayerVFX::update] Update VFX data");
            foreach (var deviceStates in devicesStates) {
                dataVFX.update_data(deviceStates.deviceId, deviceStates.verticesCountToCopy);
            }
            Profiler.EndSample();


            if (videoEndReached) {
                videoEndReachedEvent.Invoke();
                if (endAction == ActionWhenVideoEnds.DoPause) {
                    pause_video();
                } else if (endAction == ActionWhenVideoEnds.DoStop) {
                    stop_video();
                }
            }
        }

        public void set_vfx_asset(VisualEffectAsset vfx) {

            dataVFX.sharedVisualEffectAsset = vfx;

            if (is_loaded()) {
                
                int count = dataVFX.clouds_count();

                // init data and set shader variables
                dataVFX.initialize(count);

                // load display settings
                if (loadDisplaySettingsFilesAfterVideoLoading) {
                    for (int idG = 0; idG < count; ++idG) {
                        string path = string.Format("{0}_{1}.json", displaySettingsFilesBasePath, idG);
                        UnityEngine.Debug.Log(string.Format("[DCVideoPlayerVFX::set_visualeffect_asset] Load display settings file with path [{0}]. ", path));
                        dataVFX.update_display_parameters_from_json_file(idG, path);
                    }
                }
            }
        }

        #endregion


        #region mono_behaviour_functions

        private void Awake() {

            // init dll
            m_dllPlayer  = new DCVideoPlayerDLL();
            loadVideoJob = new ActionSignalJob<string>();

            // create data
            dataVFX = GetComponent<DCDataVFX>();
            if (dataVFX == null) {
                dataVFX = gameObject.AddComponent<DCDataVFX>();
            }

            // retrieve shader
            if(visualEffectAsset != null) {
                dataVFX.sharedVisualEffectAsset = visualEffectAsset;
            } else {
                dataVFX.sharedVisualEffectAsset = Resources.Load<VisualEffectAsset>("VisualEffectAssets/DCCloud");
            }
        }

  

        void Start() {

            if (loadAtStart) {
                load_video_from_file(videoFilePath);
            }
        }

        void Update() {


            // update shaders parameters
            foreach (var deviceStates in devicesStates) {
                dataVFX.update_shader_from_display_parameters(deviceStates.deviceId);
            }

            if (is_playing()) {
                update();
            }
        }

        private void OnApplicationQuit() {
            m_dllPlayer.dispose();
            m_dllPlayer = null;
        }
    }

    #endregion
}