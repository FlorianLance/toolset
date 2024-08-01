
/*******************************************************************************
** Toolset-base                                                               **
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
#include <deque>

// local
#include "dc_client_device.hpp"
#include "dc_client_processing.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"
#include "depth-camera/settings/dc_display_settings.hpp"

namespace tool::cam {

struct DCClientDeviceSettings : public io::Settings{

    DCClientDeviceSettings(){
        sType   = io::SettingsType::Dc_client_device;
        version = io::SettingsVersion::LastVersion;
    }

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    DCDeviceConnectionSettings2 connectionS;
    DCFiltersSettings filtersS;
    DCFiltersSettings calibrationFiltersS = DCFiltersSettings::default_init_for_calibration();
    DCDeviceSettings deviceS;
    DCColorSettings colorS;
    DCModelSettings modelS;
    DCDelaySettings delayS;   
    DCDeviceDisplaySettings displayS;

    // runtime
    // # paths
    std::string filtersFilePath;
    std::string calibrationFiltersFilePath;
    std::string deviceFilePath;
    std::string colorFilePath;
    std::string modelFilePath;
    // std::string delayFilePath;
    // # states
    bool connected = false;
    size_t lastFrameIdReceived = 0;
    size_t lastCompressedFrameIdReceived = 0;
    // # monitoring infos
    std::int64_t synchroAverageDiff = 0;
    net::UdpNetworkStatus receivedNetworkStatus;
    net::UdpDataStatus receivedDataStatus;

};

struct DCClientSettings : public io::Settings{

    DCClientSettings();

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    // settings
    Buffer<DCClientDeviceSettings> devicesS;

    // runtime
    std::vector<net::Interface> ipv4Interfaces = {};
    std::vector<net::Interface> ipv6Interfaces = {};
};


class DCClient{
public:

    DCClient();
    ~DCClient();

    auto initialize(const std::string &clientSettingsPath) -> bool;
    auto clean() -> void;
    auto update() -> void;

    // devices
    auto devices_nb() const noexcept -> size_t;
    auto device_connected(size_t idC) const noexcept -> bool;
    auto device_type(size_t idC) const noexcept -> DCClientType;    
    auto apply_command(size_t idC, net::Command command) -> void;
    // # remote device
    auto init_connection_with_remote_device(size_t idC) -> void;
    auto read_network_data_from_remote_device(size_t idC) -> size_t;

    // settings
    // # load
    // ## per device
    auto load_device_settings_file(size_t idC, const std::string &settingsFilePath) -> bool;
    auto load_filters_settings_file(size_t idC, const std::string &settingsFilePath) -> bool;
    auto load_calibration_filters_settings_file(size_t idC, const std::string &settingsFilePath) -> bool;
    auto load_color_filters_settings_file(size_t idC, const std::string &settingsFilePath) -> bool;
    // ## mutli devices
    auto load_device_multi_settings_file(const std::string &multiSettingsFilePath) -> bool;
    auto load_filters_multi_settings_file(const std::string &multiSettingsFilePath) -> bool;
    auto load_calibration_filters_multi_settings_file(const std::string &multiSettingsFilePath) -> bool;
    auto load_color_filters_multi_settings_file(const std::string &multiSettingsFilePath) -> bool;
    // # update
    auto use_normal_filters() -> void;
    auto use_calibration_filters() -> void;
    auto update_filters(size_t idC, const cam::DCFiltersSettings& filtersS) -> void;
    auto update_calibration_filters(size_t idC, const cam::DCFiltersSettings& filtersS) -> void;
    auto update_device_settings(size_t idC, const cam::DCDeviceSettings &deviceS) -> void;
    auto update_color_settings(size_t idC, const cam::DCColorSettings &colorS) -> void;
    auto update_delay_settings(size_t idC, const cam::DCDelaySettings &delayS) -> void;
    auto update_model_settings(size_t idC, const cam::DCModelSettings &modelS) -> void;
    // # trigger
    auto trigger_all_models_settings() -> void;
    auto trigger_all_device_display_settings() -> void;

    Buffer<std::unique_ptr<DCClientDevice>> devices;
    DCClientSettings clientS;

    // signals
    SSS<size_t, std::string> feedback_received_signal;
    SSS<size_t, std::shared_ptr<cam::DCCompressedFrame>> new_compressed_frame_signal;
    SSS<size_t, std::shared_ptr<cam::DCFrame>> new_frame_signal;
    SSS<size_t, const cam::DCModelSettings&> update_model_settings_signal;
    SSS<size_t, const cam::DCDeviceDisplaySettings&> update_device_display_settings_signal;

private:

    auto read_feedbacks() -> void;

    DCClientProcessing m_processing;

    bool m_useNormalFilteringSettings = true;
    std::mutex m_readMessagesL;
    std::deque<std::pair<size_t, net::Feedback>> m_messages;
    std::vector<std::pair<size_t, net::Feedback>> m_messagesR;
};

}
