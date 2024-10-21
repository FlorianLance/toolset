
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


    public class DLLRSMApplication {
        public DLLRSMApplication(IntPtr handle) {
            m_handle = new HandleRef(this, handle);
        }

        public void connect() {
            connect__rsm(m_handle);
        }

        private HandleRef m_handle;

        #region dll_import

        [DllImport("realstream-export", EntryPoint = "connect__rsm", CallingConvention = CallingConvention.Cdecl)]
        static private extern void connect__rsm(HandleRef rsmApp);

        #endregion
    }

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

        public void load_application(string applicationName, int appType, string jsonParametersStr) {
            load_application__qt_application_wrapper(_handle, applicationName, appType, jsonParametersStr);
        }

        public void unload_application(string applicationName) {
            unload_application__qt_application_wrapper(_handle, applicationName);
        }

        public IntPtr get_application(string applicationName) {
            return get_application__qt_application_wrapper(_handle, applicationName);
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
        static private extern void load_application__qt_application_wrapper(HandleRef qtAppW, string applicationName, int appType, string jsonParametersStr);

        [DllImport("realstream-export", EntryPoint = "unload_application__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern void unload_application__qt_application_wrapper(HandleRef qtAppW, string applicationName);

        [DllImport("realstream-export", EntryPoint = "get_application__qt_application_wrapper", CallingConvention = CallingConvention.Cdecl)]
        static private extern IntPtr get_application__qt_application_wrapper(HandleRef qtAppW, string applicationName);

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

            var basePath = Application.dataPath;
            var platform = Application.platform;
            if (platform == RuntimePlatform.WindowsPlayer || platform == RuntimePlatform.WindowsEditor) {
                exePath = basePath + "/..";
            }

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

            Debug.Log("[QtMainThread] Create DLL: " + exePath);
            dllQtAppWrapper = new DLLQtApplicationWrapper(exePath, logMessageCB);

#if UNITY_EDITOR
            EditorApplication.quitting += quit;
#endif
        }

        private void Start() {

            RealstreamManagerSettings parameters = new RealstreamManagerSettings();

            add_application("RSM1", ApplicationType.RealstreamManager, JsonUtility.ToJson(parameters));

            var rsm = new DLLRSMApplication(get_application("RSM1"));
            rsm.connect();

            //add_application("RSM2", ApplicationType.RealstreamManager

            //Debug.Log("[QtMainThread] update_application_parameters: " + JsonUtility.ToJson(parameters));
            //update_application_parameters("RSM1", JsonUtility.ToJson(parameters));
            //parameters.a = 15;
            //update_application_parameters("RSM2", JsonUtility.ToJson(parameters));
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

        public void add_application(string applicationName, ApplicationType type, string jsonParametersStr) {
            if (dllQtAppWrapper != null) {
                Debug.Log(string.Format("[QtApplicationWrapper] Load application [{0}] [{1}]", applicationName, type));
                dllQtAppWrapper.load_application(applicationName, (int)type, jsonParametersStr);
            }
        }

        public void remove_application(string applicationName) {
            if (dllQtAppWrapper != null) {
                Debug.Log(string.Format("[QtApplicationWrapper] Unload application [{0]]", applicationName));
                dllQtAppWrapper.unload_application(applicationName);
            }
        }

        public IntPtr get_application(string applicationName) {
            if (dllQtAppWrapper != null) {
                return dllQtAppWrapper.get_application(applicationName);
            }
            return IntPtr.Zero;
        }



        #endregion
    }
}