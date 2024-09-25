
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
#include <map>

// local
#include "network/network_types.hpp"
#include "network/settings/udp_server_settings.hpp"
#include "dc_filters_settings.hpp"
#include "dc_device_settings.hpp"
#include "dc_color_settings.hpp"
#include "dc_delay_settings.hpp"
#include "dc_model_settings.hpp"
#include "dc_display_settings.hpp"

namespace tool::cam {

struct ClientsRuntimeInfo{
    std::map<std::string, std::chrono::nanoseconds> clientsConnected;
    size_t lastFrameIdSent = 0;
    std::chrono::nanoseconds lastFrameSentTS = std::chrono::nanoseconds(0);
    std::int64_t lastFrameSentDurationMicroS = 0;
    std::mutex lock;
    size_t sizeDataFrame = 0;
};

struct DCServerSettings  : public io::Settings{

    DCServerSettings();
    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    auto update_reading_interface() -> void;

    // settings
    net::UdpServerSettings udpServerS;
    DCDeviceSettings deviceS = DCDeviceSettings::default_init_for_server();
    DCFiltersSettings filtersS;
    DCFiltersSettings calibrationFiltersS = DCFiltersSettings::default_init_for_calibration();
    DCColorSettings colorS;
    DCModelSettings modelS;
    DCDelaySettings delayS;
    DCDeviceDisplaySettings displayS;
    DCSceneDisplaySettings sceneDisplayS;

    // runtime
    size_t idG = 0;
    std::string globalFilePath;
    std::string deviceFilePath;
    std::string filtersFilePath;
    std::string calibrationFiltersFilePath;
    std::string colorFilePath;
    std::string modelFilePath;
    std::vector<net::Interface> ipv4Interfaces = {};
    std::vector<net::Interface> ipv6Interfaces = {};
    net::Interface udpReadingInterface;
    ClientsRuntimeInfo cInfos;
};
}
