
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

// unity
using UnityEngine;
using UnityEngine.Rendering.HighDefinition;
using UnityEngine.VFX;



#if UNITY_EDITOR
using UnityEditor;
#endif

namespace BS {

#if UNITY_EDITOR

    [CustomEditor(typeof(DCVideoRecorder))]
    public class DCVideoRecorderEditor : Editor {

        private DCVideoRecorder dcVideoRecorder { get { return target as DCVideoRecorder; } }

        public override bool RequiresConstantRepaint() {
            return true;
        }

        public override void OnInspectorGUI() {

            var origFontStyle = EditorStyles.label.fontStyle;
            EditorStyles.label.fontStyle = FontStyle.Bold;
            EditorGUILayout.LabelField("#### ACTIONS ####");
            EditorStyles.label.fontStyle = origFontStyle;

            if (!dcVideoRecorder.is_recording()) {
                if (GUILayout.Button("Start recording")) {
                    dcVideoRecorder.start_recording();
                }
            } else {
                if (GUILayout.Button("Stop recording")) {
                    dcVideoRecorder.stop_recording();
                }
            }
            if (GUILayout.Button("Reset recording")) {
                dcVideoRecorder.reset_recording();
            }

            if (GUILayout.Button("Init player from recorder")) {
                dcVideoRecorder.init_player_from_recorder();
            }  

            if (dcVideoRecorder.get_dll() != null) {
                int nbDevices = dcVideoRecorder.nb_devices();
                float durationMs = dcVideoRecorder.duration_ms();
                EditorGUILayout.LabelField(string.Format("Video duration: {0}ms",durationMs));
                EditorGUILayout.LabelField(string.Format("Nb devices: {0}", nbDevices));
                EditorGUILayout.LabelField("Nb frames:");
                for (int idD = 0; idD < nbDevices; ++idD) {
                    EditorGUILayout.LabelField(string.Format(" - device n°{0}, count; {1}", idD, dcVideoRecorder.nb_frames(idD)));
                }
            }

            base.OnInspectorGUI();
        }
    }
#endif

    public class DCVideoRecorder : MonoBehaviour {

        public DCClient associatedClient = null;
        public DCVideoPlayer associatedPlayer = null;
        public bool recordAtStart = false;

        private DCVideoRecorderDLL m_dcVideoRecorderDLL = null;

        // callbacks
        private LogMessageCB logMessageCB = null;

        #region private_functions

        #endregion

        #region public_functions

        public void start_recording() {
            if (m_dcVideoRecorderDLL != null) {
                m_dcVideoRecorderDLL.start_recording();
            }
        }

        public void stop_recording() {
            if (m_dcVideoRecorderDLL != null) {
                m_dcVideoRecorderDLL.stop_recording();
            }
        }
        public void reset_recording() {
            if (m_dcVideoRecorderDLL != null) {
                m_dcVideoRecorderDLL.reset_recording();
            }
        }

        public void associate_with_client(DCClient dcClient) {
            associatedClient = dcClient;
            if (m_dcVideoRecorderDLL != null) {
                m_dcVideoRecorderDLL.associate_with_client(associatedClient);
            }
        }

        public void dissociate_from_client() {
            if (m_dcVideoRecorderDLL != null) {
                if (associatedClient != null) {
                    m_dcVideoRecorderDLL.dissociate_from_client(associatedClient);
                }
            }
        }

        public void associate_with_player(DCVideoPlayer player) {
            if (m_dcVideoRecorderDLL != null) {
                associatedPlayer = player;
            }
        }

        public void init_player_from_recorder() {
            if(associatedPlayer != null && m_dcVideoRecorderDLL != null) {
                associatedPlayer.set_video_from_recorder(this);
            }
        }

        public bool is_recording() {
            if (m_dcVideoRecorderDLL != null) {
                return m_dcVideoRecorderDLL.is_recording();
            }
            return false;
        }

        public int nb_devices() {
            if (m_dcVideoRecorderDLL != null) {
                return m_dcVideoRecorderDLL.nb_devices();
            }
            return 0;
        }

        public int nb_frames(int idD) {
            if (m_dcVideoRecorderDLL != null) {
                return m_dcVideoRecorderDLL.nb_frames(idD);
            }
            return 0;
        }

        public float duration_ms() {
            if (m_dcVideoRecorderDLL != null) {
                return m_dcVideoRecorderDLL.duration_ms();
            }
            return 0f;
        }

        public DCVideoRecorderDLL get_dll() {
            return m_dcVideoRecorderDLL;
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


            m_dcVideoRecorderDLL = new DCVideoRecorderDLL();
            m_dcVideoRecorderDLL.init_callbacks(logMessageCB);

        }

        private void Start() {
            if(associatedClient != null) {
                m_dcVideoRecorderDLL.associate_with_client(associatedClient);
            }
        }

        private void OnApplicationQuit() {
            m_dcVideoRecorderDLL.dispose();
            m_dcVideoRecorderDLL = null;
        }

        #endregion

    }

}