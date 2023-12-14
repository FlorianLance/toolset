
#pragma once

/*******************************************************************************
** Toolset-dc-grabber                                                         **
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

// std
#include <memory>
#include <filesystem>

// base
#include "camera/settings/dc_device_settings.hpp"
#include "camera/settings/dc_display_settings.hpp"
#include "camera/settings/dc_recorder_settings.hpp"
#include "camera/settings/dc_color_settings.hpp"
#include "camera/settings/dc_delay_settings.hpp"
#include "camera/settings/dc_model_settings.hpp"
#include "camera/settings/dc_filters_settings.hpp"
#include "camera/dc_types.hpp"
#include "network/udp_client_network_settings.hpp"
#include "network/udp_header.hpp"

// local
#include "dcg_ui_settings.hpp"

namespace tool {

struct DCGSettings{

    auto initialize() -> bool;

    // settings
    auto init_network_sending_settings(std::shared_ptr<network::UdpNetworkSendingSettings> networkSendingS) -> void;
    auto update_filters(std::shared_ptr<camera::DCFiltersSettings> filtersS) -> void;
    auto update_device_settings(std::shared_ptr<network::UdpMonoPacketMessage<camera::DCDeviceSettings>> deviceS) -> void;
    auto update_color_settings(std::shared_ptr<network::UdpMonoPacketMessage<camera::DCColorSettings>> colorS) -> void;
    auto update_delay(network::UdpMonoPacketMessage<camera::DCDelaySettings> delayS) -> void;
    auto update_device_name(int id, const std::string &name) -> void;
    auto update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<std::uint8_t> value) -> void;
    auto update_imu_sample(camera::DCImuSample imuSample) -> void;

    // triggers
    auto triggers_init_network_sending_settings() -> void;
    auto triggers_filters_settings() -> void;
    auto triggers_device_settings() -> void;
    auto triggers_color_settings() -> void;
    auto triggers_display_settings() -> void;
    auto triggers_model() -> void;
    auto triggers_delay_settings() -> void;

    // network
    auto disconnect() -> void;

    // io
    // # device settings
    auto reset_device_settings() -> void;
    auto save_device_settings_to_default_file() -> bool;
    auto save_device_settings_to_current_hostname_file() -> bool;
    auto load_default_device_settings_file() -> bool;
    auto load_current_hostname_device_settings_file() -> bool;
    // # filters
    auto reset_filters() -> void;
    auto save_filters_to_default_file() -> bool;
    auto save_filters_to_current_hostname_file() -> bool;
    auto load_default_filters_file() -> bool;
    auto load_current_hostname_filters_file() -> bool;
    // # color
    auto reset_color_settings() -> void;
    auto save_color_settings_to_default_file() -> bool;
    auto save_color_settings_to_current_hostname_file() -> bool;
    auto load_default_settings_color_file() -> bool;
    auto load_current_hostname_color_settings_file() -> bool;
    // # model
    auto reset_model() -> void;
    auto save_model_to_default_file() -> bool;
    auto save_model_to_current_hostname_file() -> bool;
    auto load_default_model_file() -> bool;
    auto load_current_hostname_model_file() -> bool;

    static auto host_name() -> std::string;

    // devices
    std::vector<std::string> devicesNames;

    // settings    
    // # network
    network::UdpClientNetworkSettings network;    
    // # camera
    camera::DCDeviceSettings device = camera::DCDeviceSettings::default_init_for_grabber();
    camera::DCFiltersSettings filters;
    camera::DCColorSettings color;
    // # scene
    camera::DCSceneDisplaySettings sceneDisplay = camera::DCSceneDisplaySettings::default_init_for_grabber();
    camera::DCCloudDisplaySettings cloudDisplay = camera::DCCloudDisplaySettings::default_init_for_grabber();
    // # model
    camera::DCModelSettings model;   
    // # delay
    camera::DCDelaySettings delay;
    // # recording
    camera::DCRecorderSettings recSettings;
    camera::DCRecorderStates recStates;
    // # ui
    ui::DCGUiSettings ui;

    // data to be displayed in ui
    // # imu
    std::optional<camera::DCImuSample> imuSample;

    // files settings paths
    std::string netWorkFilePath;
    std::string deviceFilePath;
    std::string filtersFilePath;
    std::string colorFilePath;
    std::string modelFilePath;

    static inline unsigned int idLocalGrabber = 0;

private:

    std::vector<std::vector<geo::Pt2<int>>> pencils;
};
}
