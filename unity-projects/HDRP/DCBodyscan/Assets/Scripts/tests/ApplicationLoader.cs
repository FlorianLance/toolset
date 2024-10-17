
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
using System.Runtime.InteropServices;
using System;

// unity
using UnityEngine;
using UnityEditor;

namespace BS {


    public class DLLQtApplicationWrapper {

        public DLLQtApplicationWrapper(string exePath, LogMessageCB logCB) {
            _handle = new HandleRef(this, create__qt_application_wrapper(exePath, logCB));
        }
        public void dispose() {
            if (!m_disposed) {
                delete__qt_application_wrapper(_handle);
            }
            m_disposed = true;
        }

        public void quit() {
            quit__qt_application_wrapper(_handle);
        }

        public void load_application(string applicationName, int appType) {
            load_application__qt_application_wrapper(_handle, applicationName, appType);
        }

        public void unload_application(string applicationName) {
            unload_application__qt_application_wrapper(_handle, applicationName);
        }

        public void update_application_parameters(string applicationName, string jsonParametersStr) {
            update_application_parameter_from_json_str__qt_application_wrapper(_handle, applicationName, jsonParametersStr);
        }

        ~DLLQtApplicationWrapper() {
            dispose();
        }

        protected HandleRef _handle;
        protected bool m_disposed = false;

        #region dll_import


        [DllImport("realstream-export", EntryPoint = "create__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern IntPtr create__qt_application_wrapper(string execPath, [MarshalAs(UnmanagedType.FunctionPtr)] LogMessageCB logMessageCB);
        
        [DllImport("realstream-export", EntryPoint = "delete__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern void delete__qt_application_wrapper(HandleRef qtAppW);

        [DllImport("realstream-export", EntryPoint = "quit__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern void quit__qt_application_wrapper(HandleRef qtAppW);

        [DllImport("realstream-export", EntryPoint = "load_application__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern void load_application__qt_application_wrapper(HandleRef qtAppW, string applicationName, int appType);

        [DllImport("realstream-export", EntryPoint = "unload_application__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern void unload_application__qt_application_wrapper(HandleRef qtAppW, string applicationName);

        [DllImport("realstream-export", EntryPoint = "update_application_parameter_from_json_str__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern void update_application_parameter_from_json_str__qt_application_wrapper(HandleRef qtAppW, string applicationName, string jsonStrUTF8);

        #endregion
    }




    [Serializable]
    public class RealstreamManagerSettings {
        public string clientJSONFilePath = "E:/_json/client_local1_test.json";
        public int a = 2;
        //public Dictionary<string, object> parameters = new Dictionary<string, object>();
    }

    public class ApplicationLoader : MonoBehaviour {
        public enum ApplicationType {
            RealstreamManager
        };


        private DLLQtApplicationWrapper dllQtAppWrapper = null;
        private LogMessageCB logMessageCB = null;

        #region private_functions

        public void quit() {
            if (dllQtAppWrapper != null) {
                Debug.Log("[QtApplicationWrapper] Quit");
                dllQtAppWrapper.quit();
            }
        }

        #endregion

        #region mono_behaviour_functions

        private void Awake() {

            string exePath = "";

            Debug.Log("[QtMainThread] Init callbacks");
            logMessageCB = (string message, int type) => {
                if (type == 0) {
                    UnityEngine.Debug.Log(message);
                } else if (type == 1) {
                    UnityEngine.Debug.LogWarning(message);
                } else if (type == 2) {
                    UnityEngine.Debug.LogError(message);
                }
            };

            Debug.Log("[QtMainThread] Create DLL");
            dllQtAppWrapper = new DLLQtApplicationWrapper(exePath, logMessageCB);

#if UNITY_EDITOR
            EditorApplication.quitting += quit;
#endif
        }

        private void Start() {
            add_application("RSM1", ApplicationType.RealstreamManager);
            add_application("RSM2", ApplicationType.RealstreamManager);

            RealstreamManagerSettings parameters = new RealstreamManagerSettings();
            Debug.Log("[QtMainThread] update_application_parameters: " + JsonUtility.ToJson(parameters));
            update_application_parameters("RSM1", JsonUtility.ToJson(parameters));
            parameters.a = 15;
            update_application_parameters("RSM2", JsonUtility.ToJson(parameters));
        }

        private void OnApplicationQuit() {

#if UNITY_EDITOR

#else
            quit();
#endif
            Debug.Log("[QtApplicationWrapper] Dispose");
            dllQtAppWrapper.dispose();

            Debug.Log("[QtApplicationWrapper] Destroy");
            dllQtAppWrapper = null;
        }

#endregion

        #region public_functions

        public void add_application(string applicationName, ApplicationType type) {
            if (dllQtAppWrapper != null) {
                Debug.Log(string.Format("[QtApplicationWrapper] Load application [{0}] [{1}]", applicationName, type));
                dllQtAppWrapper.load_application(applicationName, (int)type);
            }
        }

        public void remove_application(string applicationName) {
            if (dllQtAppWrapper != null) {
                Debug.Log(string.Format("[QtApplicationWrapper] Unload application [{0]]", applicationName));
                dllQtAppWrapper.unload_application(applicationName);
            }
        }

        public void update_application_parameters(string applicationName, string jsonParametersStr) {
            if (dllQtAppWrapper != null) {
                dllQtAppWrapper.update_application_parameters(applicationName, jsonParametersStr);
            }
        }

        #endregion
    }
}