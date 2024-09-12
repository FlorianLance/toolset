

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
using UnityEngine.UIElements;

namespace BS {

    public class DCVideoRecorderDLL {

        public DCVideoRecorderDLL() {
            _handle = new HandleRef(this, create__dc_video_recorder_export());
        }

        public void dispose() {
            if (!m_disposed) {
                delete__dc_video_recorder_export(_handle);
            }
            m_disposed = true;
        }

        ~DCVideoRecorderDLL() {
            dispose();
        }

        protected HandleRef _handle;
        protected bool m_disposed = false;

        public void init_callbacks(LogMessageCB logCB) {
            init_callbacks__dc_video_recorder_export(_handle, logCB);
        }

        public void associate_with_client(DCClient client) {
            associate_with_client__dc_video_recorder_export(_handle, client.get_dll().get_handle());
        }

        public void dissociate_from_client(DCClient client) {
            dissociate_from_client__dc_video_recorder_export(_handle, client.get_dll().get_handle());
        }
        public HandleRef get_handle() {
            return _handle;
        }

        public void start_recording() {
            start_recording__dc_video_recorder_export(_handle);
        }
        public void stop_recording() {
            stop_recording__dc_video_recorder_export(_handle);
        }
        public void reset_recording() {
            reset_recording__dc_video_recorder_export(_handle);
        }
        
        public bool is_recording() {
            return is_recording__dc_video_recorder_export(_handle) == 1;
        }

        public int nb_devices() {
            return nb_devices__dc_video_recorder_export(_handle);
        }

        public int nb_frames(int idD) {
            return nb_frames__dc_video_recorder_export(_handle, idD);
        }

        public float duration_ms() {
            return duration__dc_video_recorder_export(_handle);
        }

        #region dll_import

        // create / delete / init
        [DllImport("base-export", EntryPoint = "create__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern IntPtr create__dc_video_recorder_export();
        [DllImport("base-export", EntryPoint = "delete__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void delete__dc_video_recorder_export(HandleRef dcVideoRecorderExport);

        [DllImport("base-export", EntryPoint = "init_callbacks__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void init_callbacks__dc_video_recorder_export(HandleRef dcVideoRecorderExport, [MarshalAs(UnmanagedType.FunctionPtr)] LogMessageCB logMessageCB);
        [DllImport("base-export", EntryPoint = "associate_with_client__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void associate_with_client__dc_video_recorder_export(HandleRef dcVideoRecorderExport, HandleRef dcClientExport);

        [DllImport("base-export", EntryPoint = "dissociate_from_client__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void dissociate_from_client__dc_video_recorder_export(HandleRef dcVideoRecorderExport, HandleRef dcClientExport);

        // actions
        [DllImport("base-export", EntryPoint = "start_recording__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void start_recording__dc_video_recorder_export(HandleRef dcVideoRecorderExport);
        [DllImport("base-export", EntryPoint = "stop_recording__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void stop_recording__dc_video_recorder_export(HandleRef dcVideoRecorderExport);
        [DllImport("base-export", EntryPoint = "reset_recording__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern void reset_recording__dc_video_recorder_export(HandleRef dcVideoRecorderExport);

        // data
        
        [DllImport("base-export", EntryPoint = "is_recording__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int is_recording__dc_video_recorder_export(HandleRef dcVideoRecorderExport);
        [DllImport("base-export", EntryPoint = "nb_devices__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int nb_devices__dc_video_recorder_export(HandleRef dcVideoRecorderExport);

        [DllImport("base-export", EntryPoint = "nb_frames__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern int nb_frames__dc_video_recorder_export(HandleRef dcVideoRecorderExport, int idD);

        [DllImport("base-export", EntryPoint = "duration__dc_video_recorder_export", CallingConvention = CallingConvention.Cdecl)]
        static private extern float duration__dc_video_recorder_export(HandleRef dcVideoRecorderExport);
        #endregion
    }

}
