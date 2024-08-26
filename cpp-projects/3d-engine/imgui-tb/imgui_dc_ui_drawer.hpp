
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

// std
#include <map>

// base
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_calibrator_settings.hpp"
#include "depth-camera/settings/dc_display_settings.hpp"
#include "depth-camera/settings/dc_video_recorder_settings.hpp"
#include "depth-camera/settings/dc_video_player_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"
#include "depth-camera/states/dc_video_player_states.hpp"
#include "depth-camera/states/dc_video_recorder_states.hpp"
#include "depth-camera/states/dc_calibrator_states.hpp"

// local
#include "imgui_types.hpp"

namespace tool::graphics {

[[maybe_unused]] static constexpr const char* devicesTypes[] = {
    "Azure kinect", "Femto bolt", "Femto mega"
};

[[maybe_unused]] static constexpr const char* synchItems[] = {
    "Standalone", "Primary", "Subordinate"
};

[[maybe_unused]] static constexpr const char* k4PowerlineFrequencyItems[] = {
    "50", "60"
};
[[maybe_unused]] static constexpr const char* obPowerlineFrequencyItems[] = {
    "Close","50", "60"
};

[[maybe_unused]] static constexpr const char* sizesPencilItems[] = {
    "10", "40", "100"
};

[[maybe_unused]] static constexpr const char* processingModeBodyTrackingItems[] = {
    "GPU", "CPU", "GPU_CUDA", "GPU_TENSORRT", "GPU_DIRECTML"
};

[[maybe_unused]] static constexpr const char* orientationBodyTrackingItems[] = {
    "Default", "Clockwise 90°", "Counter clockwise 90", "Flip 180°"
};

[[maybe_unused]] static constexpr const char* protocolItems[] = {
    "IPV4", "IVP6"
};



class DCUIDrawer{

public:

    static auto set_manager_mode(bool state) -> void{
        m_isManager = true;
    }

    static auto draw_dc_config(cam::DCConfigSettings &config) -> bool;
    static auto draw_dc_data_settings(cam::DCType type, cam::DCDataSettings &data) -> bool;
    static auto draw_dc_device_settings_tab_item( const std::string &tabItemName, cam::DCDeviceSettings &device) -> std::tuple<bool,bool>;

    static auto draw_dc_colors_settings_tab_item(const std::string &tabItemName, cam::DCType type, cam::DCColorSettings &colors) -> std::tuple<bool,bool>;
    static auto draw_dc_filters_settings_tab_item(const std::string &tabItemName, cam::DCMode mode, cam::DCFiltersSettings &filters) -> std::tuple<bool, bool>;
    static auto draw_dc_scene_display_setings_tab_item(const std::string &tabItemName, cam::DCSceneDisplaySettings &display) -> bool;
    static auto draw_dc_cloud_display_setings_tab_item(const std::string &tabItemName, cam::DCDeviceDisplaySettings &display) -> bool;
    static auto draw_dc_model_tab_item(const std::string &tabItemName, cam::DCModelSettings &model) -> bool;
    static auto draw_dc_delay_tab_item(const std::string &tabItemName, cam::DCDelaySettings &delayS) -> bool;
    static auto draw_dc_recorder_tab_item(const std::string &tabItemName, cam::DCVideoRecorderStates &rStates, cam::DCVideoRecorderSettings &rSettings) -> bool;
    static auto draw_dc_player_tab_item(const std::string &tabItemName, cam::DCVideoPlayerStates &pStates, cam::DCVideoPlayerSettings &pSettings) -> bool;
    static auto draw_dc_calibrator_tab_item(const std::string &tabItemName, bool useNormalFilteringSettings, cam::DCCalibratorStates &cStates, cam::DCCalibratorDisplaySettings &cdSettings, cam::DCCalibratorSettings &cSettings) -> bool;

    static inline ImGuiDragS  modelRotDs = {45.f, true, true, false, true, true};
    static inline ImGuiDragS  modelTrDs  = {45.f, true, true, false, true, true};
    static inline ImGuiDragS  modelScDs  = {45.f, true, true, false, true, true};
    static inline ImGuiFloatS modelRotFs = {0.f, -360.f, 360.f, 0.01f, 0.01f, "%.2f"};
    static inline ImGuiFloatS modelTrFs  = {0.f, -10.f, 10.f, 0.001f, 0.001f, "%.3f"};
    static inline ImGuiFloatS modelScFs  = {1.f, -100.f, 100.f, 0.01f, 0.01f, "%.3f"};

    static inline ImGuiFloatS alignColDepthTrFs  = {0.f, -50.f, 50.f, 1.f, 0.1f, "%.1f"};

    static inline std::vector<std::string> devicesNames = {
        "dev_0", "dev_1", "dev_2", "dev_3", "dev_4", "dev_5", "dev_6", "dev_7", "dev_8", "dev_9"};
    static inline std::vector<cam::DCMode> k4Modes = cam::dc_get_device_modes(cam::DCType::AzureKinect);
    static inline std::vector<cam::DCMode> fbModes = cam::dc_get_device_modes(cam::DCType::FemtoBolt);
    static inline std::vector<cam::DCMode> fmModes = cam::dc_get_device_modes(cam::DCType::FemtoMega);
    static inline std::map<cam::DCMode, std::string> modesNames = {};

    static auto udpate_device_name(size_t idDevice, const std::string &deviceName) -> void{
        if(idDevice > devicesNames.size()){
            devicesNames.resize(idDevice);
        }
        devicesNames[idDevice] = deviceName;
    }

    static inline bool m_isManager = false;
};

}



