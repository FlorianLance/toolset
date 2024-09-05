

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
using System;
using System.Runtime.InteropServices;

namespace BS {


    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void LogMessageCB(string message, int type);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void NewFeedbackCB(int idC, int messageType, int feedbackType);    

    public class DCClientDLL {

        public DCClientDLL() {
            _handle = new HandleRef(this, create__dc_client_export());
        }

        public void dispose() {
            if (!m_disposed) {
                delete__dc_client_export(_handle);
            }
            m_disposed = true;
        }

        ~DCClientDLL() {
            dispose();
        }

        protected HandleRef _handle;
        protected bool m_disposed = false;

        // init
        public void init_callbacks(LogMessageCB logCB, NewFeedbackCB newFeedbackCB) {
            init_callbacks__dc_client_export(_handle,
                logCB,
                newFeedbackCB);
        }
        public bool initialize(string clientSettingsFilePath, bool startThreads) {
            return initialize__dc_client_export(_handle, clientSettingsFilePath, startThreads ? 1 : 0) == 1;
        }

        // update
        // # main thread
        public void update() {
            update__dc_client_export(_handle);
        }
        // # dedicated thread
        public int read_data_from_external_thread(int idD) {
            return read_data_from_external_thread__dc_client_export(_handle, idD);
        }
        public void trigger_packets_from_external_thread(int idD) {
            trigger_packets_from_external_thread__dc_client_export(_handle, idD);
        }
        public void process_frames_from_external_thread(int idD) {
            process_frames_from_external_thread__dc_client_export(_handle, idD);
        }

        // actions
        public void connect_to_server(int idD) {
            connect_to_server__dc_client_export(_handle, idD);
        }
        public void connect_to_devices() {
            connect_to_devices__dc_client_export(_handle);
        }

        public void disconnect_from_devices() {
            disconnect_from_devices__dc_client_export(_handle);
        }


        // states
        public bool is_local(int idD) {
            return is_local__dc_client_export(_handle, idD) == 1;
        }
        public int devices_nb() {
            return devices_nb__dc_client_export(_handle);
        }
        public bool is_device_connected(int idD) {
            return is_device_connected__dc_client_export(_handle, idD) == 1;
        }
        public int current_frame_id(int idD) {
            return current_frame_id__dc_client_export(_handle, idD);
        }
        public int current_frame_cloud_size(int idD) {
            return current_frame_cloud_size__dc_client_export(_handle, idD);
        }

        // settings
        public void apply_devices_settings(int idD) {
            apply_device_settings__dc_client_export(_handle, idD);
        }
        public void apply_color_settings() {
            apply_color_settings__dc_client_export(_handle);
        }
        public void apply_filters_settings() {
            apply_filters_settings__dc_client_export(_handle);
        }

        // frames
        public bool is_frame_available(int idD) {
            return is_frame_available__dc_client_export(_handle, idD) == 1;
        }

        public void invalidate_frame(int idD) {
            invalidate_frame__dc_client_export(_handle, idD);
        }

        // data
        public void copy_transform(int idD, float[] transformData) {
            copy_transform__dc_client_export(_handle, idD, transformData);
        }

        public int copy_current_frame_vertices_vfx(int idCamera, BS.NativeDLLVec3 positions, BS.NativeDLLVec3 colors, BS.NativeDLLVec3 normals, int verticesCount, bool applyModelTransform) {
            int nbVerticesCopied = 0;
            unsafe {
                nbVerticesCopied = copy_current_frame_vertices_vfx__dc_client_export(_handle,
                    idCamera,
                    Unity.Collections.LowLevel.Unsafe.NativeArrayUnsafeUtility.GetUnsafePtr(positions.native),
                    Unity.Collections.LowLevel.Unsafe.NativeArrayUnsafeUtility.GetUnsafePtr(colors.native),
                    Unity.Collections.LowLevel.Unsafe.NativeArrayUnsafeUtility.GetUnsafePtr(normals.native),
                    verticesCount, applyModelTransform ? 1 : 0
                );
            }
            return nbVerticesCopied;
        }

        #region dll_import

        // create / delete
        [DllImport("base-export", EntryPoint = "create__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern IntPtr create__dc_client_export();
        [DllImport("base-export", EntryPoint = "delete__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void delete__dc_client_export(HandleRef dcNetworkDirectPlayer);

        // init
        [DllImport("base-export", EntryPoint = "init_callbacks__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void init_callbacks__dc_client_export(HandleRef dcNetworkDirectPlayer,
            [MarshalAs(UnmanagedType.FunctionPtr)] LogMessageCB logMessageCB,
            [MarshalAs(UnmanagedType.FunctionPtr)] NewFeedbackCB newFeedbackCB
        );
        [DllImport("base-export", EntryPoint = "initialize__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int initialize__dc_client_export(HandleRef dcNetworkDirectPlayer, string networkSettingsFilePath, int startThreads);

        // update
        // # main thread
        [DllImport("base-export", EntryPoint = "update__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int update__dc_client_export(HandleRef dcNetworkDirectPlayer);
        // # dedicated thread
        [DllImport("base-export", EntryPoint = "read_data_from_external_thread__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int read_data_from_external_thread__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);        
        [DllImport("base-export", EntryPoint = "trigger_packets_from_external_thread__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void trigger_packets_from_external_thread__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);
        [DllImport("base-export", EntryPoint = "process_frames_from_external_thread__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void process_frames_from_external_thread__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);

        // actions        
        [DllImport("base-export", EntryPoint = "connect_to_server__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void connect_to_server__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);
        [DllImport("base-export", EntryPoint = "connect_to_devices__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void connect_to_devices__dc_client_export(HandleRef dcNetworkDirectPlayer);
        [DllImport("base-export", EntryPoint = "disconnect_from_devices__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void disconnect_from_devices__dc_client_export(HandleRef dcNetworkDirectPlayer);

        // states
        [DllImport("base-export", EntryPoint = "is_local__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int is_local__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);
        [DllImport("base-export", EntryPoint = "devices_nb__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int devices_nb__dc_client_export(HandleRef dcNetworkDirectPlayer);
        [DllImport("base-export", EntryPoint = "is_device_connected__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int is_device_connected__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);
        [DllImport("base-export", EntryPoint = "current_frame_id__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int current_frame_id__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);
        [DllImport("base-export", EntryPoint = "current_frame_cloud_size__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int current_frame_cloud_size__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);

        // settings
        [DllImport("base-export", EntryPoint = "apply_device_settings__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void apply_device_settings__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);
        [DllImport("base-export", EntryPoint = "apply_color_settings__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void apply_color_settings__dc_client_export(HandleRef dcNetworkDirectPlayer);
        [DllImport("base-export", EntryPoint = "apply_filters_settings__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void apply_filters_settings__dc_client_export(HandleRef dcNetworkDirectPlayer);

        // frames
        [DllImport("base-export", EntryPoint = "is_frame_available__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int is_frame_available__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);

        [DllImport("base-export", EntryPoint = "invalidate_frame__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void invalidate_frame__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD);

        // data
        [DllImport("base-export", EntryPoint = "copy_transform__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void copy_transform__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD, float[] transformData);
        [DllImport("base-export", EntryPoint = "copy_current_frame_vertices__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        unsafe static private extern int copy_current_frame_vertices__dc_client_export(HandleRef dcNetworkDirectPlayer, int idD, void* vertices, int verticesCount, int applyModelTransform);

        [DllImport("base-export", EntryPoint = "copy_current_frame_vertices_vfx__dc_client_export", CallingConvention = CallingConvention.Cdecl)]
        unsafe static public extern int copy_current_frame_vertices_vfx__dc_client_export(HandleRef dcPlayer, int idCamera, void* positions, void* colors, void* normals, int verticesCount, int applyModelTransform);


        #endregion
    }
}




