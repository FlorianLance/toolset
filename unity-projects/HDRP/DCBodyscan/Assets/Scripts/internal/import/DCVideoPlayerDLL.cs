

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

    public class DCVideoPlayerDLL {

        public DCVideoPlayerDLL() {
            _handle = new HandleRef(this, create__dc_video_player());
        }

        public void dispose() {
            if (!m_disposed) {
                delete__dc_video_player(_handle);
            }
            m_disposed = true;
        }

        ~DCVideoPlayerDLL() {
            dispose();
        }

        protected HandleRef _handle;
        protected bool m_disposed = false;

        // I/O
        public bool load_from_file(string videoFilePath) { return load_from_file__dc_video_player(_handle, videoFilePath) == 1; }

        public bool set_video_from_recorder(DCVideoRecorder dCVideoRecorder) {
            set_video_from_recorder__dc_video_player(_handle, dCVideoRecorder.get_dll().get_handle());
            return true;
        }

        // settings
        public void set_player_settings(bool doLoops, float startTimeMs, float endTimeMs) { set_player_settings__dc_video_player(_handle, doLoops ? 1 : 0, startTimeMs, endTimeMs); }

        // navigation
        public bool is_started() { return is_started__dc_video_player(_handle) == 1; }
        public bool is_playing() { return is_playing__dc_video_player(_handle) == 1; }
        public bool is_looping() { return is_looping__dc_video_player(_handle) == 1; }
        public float start_time_ms() { return start_time_ms__dc_video_player(_handle); }
        public float end_time_ms() { return end_time_ms__dc_video_player(_handle); }
        public float current_time_ms() { return current_time_ms__dc_video_player(_handle); }
        public float duration_ms() { return duration_ms__dc_video_player(_handle); }
        public void start_video() { start_video__dc_video_player(_handle); }
        public void stop_video() { stop_video__dc_video_player(_handle); }
        public void pause_video() { pause_video__dc_video_player(_handle); }
        public void unpause_video() { unpause_video__dc_video_player(_handle); }
        public void go_to_start_time() { go_to_start_time__dc_video_player(_handle); }
        public void go_to_end_time() { go_to_end_time__dc_video_player(_handle); }
        public void set_current_time(float timeMs) { set_current_time__dc_video_player(_handle, timeMs); }
        public void update() { update__dc_video_player(_handle); }

        // data
        public int cameras_count() { return cameras_count__dc_video_player(_handle); }
        public int frames_count(int idCamera) { return frames_count__dc_video_player(_handle, idCamera); }
        public int current_frame_id(int idCamera) { return current_frame_id__dc_video_player(_handle, idCamera); }
        public int get_current_frame_valid_vertices_count(int idCamera) { return get_current_frame_valid_vertices_count__dc_video_player(_handle, idCamera); }
        public int get_current_compressed_frame_valid_vertices_count(int idCamera) { return get_current_compressed_frame_valid_vertices_count__dc_video_player(_handle, idCamera); }
        public int get_current_frames_total_cloud_size() { return get_current_frames_total_cloud_size__dc_video_player(_handle); }
        public void copy_transform(int idCamera, float[] transformData) { get_transform__dc_video_player(_handle, idCamera, transformData); }
        public int copy_camera_cloud_vfx(int idCamera, BS.NativeDLLVec3 positions, BS.NativeDLLVec3 colors, BS.NativeDLLVec3 normals, int verticesCount, bool applyModelTransform) {
            int nbVerticesCopied = 0;
            unsafe {
                nbVerticesCopied = copy_camera_cloud_vfx__dc_video_player(_handle,
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

        [DllImport("base-export", EntryPoint = "create__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern IntPtr create__dc_video_player();
        [DllImport("base-export", EntryPoint = "delete__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern void delete__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "load_from_file__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern int load_from_file__dc_video_player(HandleRef dcPlayer, string pathDcVideoFile);
        [DllImport("base-export", EntryPoint = "set_video_from_recorder__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern void set_video_from_recorder__dc_video_player(HandleRef dcPlayer, HandleRef dcRecorderExport);

        // navigation
        [DllImport("base-export", EntryPoint = "is_started__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern int is_started__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "is_playing__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern int is_playing__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "is_looping__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern int is_looping__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "start_time_ms__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern float start_time_ms__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "end_time_ms__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern float end_time_ms__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "duration_ms__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern float duration_ms__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "current_time_ms__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern float current_time_ms__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "set_player_settings__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern void set_player_settings__dc_video_player(HandleRef dcPlayer, int doLoops, float startTimeMs, float endTimeMs);

        [DllImport("base-export", EntryPoint = "start_video__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void start_video__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "stop_video__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void stop_video__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "pause_video__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void pause_video__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "unpause_video__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void unpause_video__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "go_to_start_time__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void go_to_start_time__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "go_to_end_time__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void go_to_end_time__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "set_current_time__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void set_current_time__dc_video_player(HandleRef dcPlayer, float timeMs);
        [DllImport("base-export", EntryPoint = "update__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern void update__dc_video_player(HandleRef dcPlayer);

        // data
        [DllImport("base-export", EntryPoint = "cameras_count__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern int cameras_count__dc_video_player(HandleRef dcPlayer);
        [DllImport("base-export", EntryPoint = "frames_count__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern int frames_count__dc_video_player(HandleRef dcPlayer, int idCamera);
        [DllImport("base-export", EntryPoint = "current_frame_id__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern int current_frame_id__dc_video_player(HandleRef dcPlayer, int idCamera);
        [DllImport("base-export", EntryPoint = "get_transform__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static private extern void get_transform__dc_video_player(HandleRef dcPlayer, int idCamera, float[] transformData);

        [DllImport("base-export", EntryPoint = "get_current_frame_valid_vertices_count__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern int get_current_frame_valid_vertices_count__dc_video_player(HandleRef dcPlayer, int idCamera);

        [DllImport("base-export", EntryPoint = "get_current_compressed_frame_valid_vertices_count__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern int get_current_compressed_frame_valid_vertices_count__dc_video_player(HandleRef dcPlayer, int idCamera);

        [DllImport("base-export", EntryPoint = "get_current_frames_total_cloud_size__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        static public extern int get_current_frames_total_cloud_size__dc_video_player(HandleRef dcPlayer);

        [DllImport("base-export", EntryPoint = "copy_camera_cloud_vfx__dc_video_player", CallingConvention = CallingConvention.Cdecl)]
        unsafe static public extern int copy_camera_cloud_vfx__dc_video_player(HandleRef dcPlayer, int idCamera, void* positions, void* colors, void* normals, int verticesCount, int applyModelTransform);

        #endregion
    }
}




