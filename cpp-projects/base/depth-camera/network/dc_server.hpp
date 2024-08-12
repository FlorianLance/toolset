
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


// local
#include "thirdparty/sigslot/signal.hpp"
#include "network/network_types.hpp"
#include "network/settings/udp_server_settings.hpp"
#include "network/settings/udp_connection_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"
#include "depth-camera/frame/dc_compressed_frame.hpp"

namespace tool::net {
struct DCServerSettings  : public io::Settings{

    DCServerSettings();
    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    auto update_reading_interface() -> void;

    // settings
    net::UdpServerSettings udpServerS;
    cam::DCDeviceSettings deviceS = cam::DCDeviceSettings::default_init_for_grabber();
    cam::DCFiltersSettings filtersS;
    cam::DCFiltersSettings calibrationFiltersS = cam::DCFiltersSettings::default_init_for_calibration();
    cam::DCColorSettings colorS;
    cam::DCModelSettings modelS;
    cam::DCDelaySettings delayS;

    // runtime
    std::string globalFilePath;
    std::string deviceFilePath;
    std::string filtersFilePath;
    std::string calibrationFiltersFilePath;
    std::string colorFilePath;
    std::string modelFilePath;
    std::vector<net::Interface> ipv4Interfaces = {};
    std::vector<net::Interface> ipv6Interfaces = {};
    net::Interface udpReadingInterface;
};


class DCServer{
public:

    DCServer();
    ~DCServer();

    auto initialize(const std::string &serverSettingsPath) -> bool;
    auto legacy_initialize(const std::string &legacyNetworkSettingsFilePath) -> bool;
    auto clean() -> void;
    auto update() -> void;

    // settings
    auto load_device_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_filters_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_calibration_filters_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_color_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_model_settings_file(const std::string &settingsFilePath) -> bool;

    // actions
    auto send_frame(std::shared_ptr<cam::DCCompressedFrame> frame) -> void;
    // auto send_feedback(Feedback feedback) -> void;

    // others
    auto ping_server() -> void;
    auto simulate_sending_failure(bool enabled, int percentage) -> void; // TODO

    // monitoring
    auto last_frame_id_sent() const -> size_t;
    auto last_frame_sent_timestamp_nanosecond() const -> std::chrono::nanoseconds;
    auto last_frame_sending_duration_micros_s() const -> std::int64_t;

    DCServerSettings settings;

    // signals
    // # connection
    sigslot::signal<UdpConnectionSettings> receive_init_server_client_connection_signal;
    // # settings received
    sigslot::signal<std::shared_ptr<cam::DCDeviceSettings>> receive_device_settings_signal;
    sigslot::signal<std::shared_ptr<cam::DCColorSettings>> receive_color_settings_signal;
    sigslot::signal<std::shared_ptr<cam::DCFiltersSettings>> receive_filters_signal;
    sigslot::signal<cam::DCDelaySettings> receive_delay_settings_signal;
    // # command received
    sigslot::signal<> shutdown_signal;
    sigslot::signal<> quit_signal;
    sigslot::signal<> restart_signal;
    // # other
    sigslot::signal<size_t> timeout_messages_signal;

private:


    struct Impl;
    std::unique_ptr<Impl> i;
};

}
