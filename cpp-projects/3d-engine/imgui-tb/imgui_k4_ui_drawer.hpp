
/*******************************************************************************
** Toolset-3d-engine                                                          **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#pragma once

// base
#include "camera/kinect4/k4_device_settings.hpp"
#include "camera/kinect4/k4_filters.hpp"
#include "camera/kinect4/k4_display_settings.hpp"
#include "camera/kinect4/k4_recorder_settings.hpp"
#include "camera/kinect4/k4_player_settings.hpp"
#include "camera/kinect4/k4_calibrator_settings.hpp"
#include "camera/kinect4/k4_color_settings.hpp"
#include "camera/kinect4/k4_model.hpp"

// opengl
#include "opengl/draw/drawer.hpp"

// local
#include "imgui_types.hpp"

namespace tool::graphics {

[[maybe_unused]] static constexpr const char* modeItems[] = {
    "Cloud_320x288",
    "Cloud_640x576_MJPEG","Cloud_640x576_YUY2", "Cloud_640x576_NV12", "Cloud_640x576_BGRA32",
    "Cloud_512x512", "Cloud_1024x1024",
    "Full_frame_320x288", "Full_frame_640x576", "Full_frame_512x512", "Full_frame_1024x1024",
    "Only_color_1280x720", "Only_color_1920x1080", "Only_color_2560x1440", "Only_color_2048x1536", "Only_color_3840x2160", "Only_color_4096x3072"
};

[[maybe_unused]] static constexpr const char* compressModeItems[] = {
    "Full", "Cloud", "None"
};

[[maybe_unused]] static constexpr const char* synchItems[] = {
    "Standalone", "Master", "Subordinate"
};

[[maybe_unused]] static constexpr const char* powerlineFrequencyItems[] = {
    "50", "60"
};

[[maybe_unused]] static constexpr const char* sizesPencilItems[] = {
    "10", "40", "100"
};

[[maybe_unused]] static constexpr const char* exposureTimesMicroSTimes[] = {
    "500", "1250", "2500", "8330", "16670", "33330"
};

class K4UIDrawer{

public:

    static auto draw_config(const std::vector<std::string> &devicesName, camera::K4ConfigSettings &config, bool &updateDeviceList, bool &updateP) -> void;
    static auto draw_data_settings(camera::K4DataSettings &data, bool &updateP) -> void;
    static auto draw_actions_settings(camera::K4ActionsSettings &actions,  bool &updateP) -> void;
    static auto draw_device_settings_tab_item(
        const std::string &tabItemName,
        const std::vector<std::string> &devicesName,
        camera::K4DeviceSettings &device,
        bool &updateDeviceList,
        bool &autoUpdate
    ) -> bool;

    static auto draw_colors_settings_tab_item(const std::string &tabItemName, camera::K4ColorSettings &colors, bool &autoUpdate) -> bool;
    static auto draw_filters_tab_item(const std::string &tabItemName, camera::K4Mode mode, camera::K4Filters &filters, bool &autoUpdate) -> std::tuple<bool,bool>;
    static auto draw_scene_display_setings_tab_item(const std::string &tabItemName, camera::K4SceneDisplaySettings &display, bool &autoUpdate) -> bool;
    static auto draw_cloud_display_setings_tab_item(const std::string &tabItemName, camera::K4CloudDisplaySettings &display, bool &autoUpdate) -> bool;
    static auto draw_calibration_tab_item(const std::string &tabItemName, camera::K4Model &model, bool &autoUpdate) -> bool;
    static auto draw_recording_tab_item(const std::string &tabItemName, camera::K4RecorderStates &rStates, camera::K4RecorderSettings &rSettings, bool &autoUpdate) -> bool;
    static auto draw_player_tab_item(const std::string &tabItemName, camera::K4PlayerStates &pStates, camera::K4PlayerSettings &pSettings, bool &autoUpdate) -> bool;
    static auto draw_calibrator_tab_item(const std::string &tabItemName, bool useNormalFilteringSettings, camera::K4CalibratorStates &cStates, K4CalibratorDrawerSettings &cdSettings, camera::K4CalibratorSettings &cSettings, bool &autoUpdate) -> bool;

    static inline ImGuiDragS  modelRotDs = {45.f, true, true, false, true, true};
    static inline ImGuiDragS  modelTrDs  = {45.f, true, true, false, true, true};
    static inline ImGuiDragS  modelScDs  = {45.f, true, true, false, true, true};
    static inline ImGuiFloatS modelRotFs = {0.f, -360.f, 360.f, 0.01f, 0.01f, "%.2f"};
    static inline ImGuiFloatS modelTrFs  = {0.f, -10.f, 10.f, 0.001f, 0.001f, "%.3f"};
    static inline ImGuiFloatS modelScFs  = {1.f, -100.f, 100.f, 0.01f, 0.01f, "%.3f"};
};


struct CloudPointsFrameDrawer{
    int currentFrameId = 0;
    int startFrameId = 0;
    int endFrameId = 0;
    gl::CloudPointsDrawer cloudD;
};

}



