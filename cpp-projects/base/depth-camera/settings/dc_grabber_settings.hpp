

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
#include "network/network_types.hpp"
#include "dc_grabber_network_settings.hpp"
#include "dc_filters_settings.hpp"
#include "dc_device_settings.hpp"
#include "dc_color_settings.hpp"
#include "dc_model_settings.hpp"
#include "dc_delay_settings.hpp"
#include "dc_display_settings.hpp"

namespace tool::cam {

struct DCMGrabberSettings{

    auto initialize(size_t idC, const net::ReadSendNetworkInfos &clientInfos) -> void;

    size_t id = 0;
    DCMGrabberNetworkSettings network;

    DCFiltersSettings filters;
    DCFiltersSettings calibrationFilters = DCFiltersSettings::default_init_for_calibration();
    DCDeviceSettings device;
    DCColorSettings color;
    DCModelSettings model;
    DCDelaySettings delay;

    std::string filtersFilePath;
    std::string calibrationFiltersFilePath;
    std::string deviceFilePath;
    std::string colorFilePath;
    std::string modelFilePath;

    DCCloudDisplaySettings cloudDisplay;

    std::int64_t synchroAverageDiff = 0;
    net::UdpReceivedStatus receivedStatus;

    static const inline std::vector<geo::Pt4f> cloudsColors = {
        {1.0f,0.0f,0.0f, 1.0f},
        {0.0f,1.0f,0.0f, 1.0f},
        {0.0f,0.0f,1.0f, 1.0f},
        {1.0f,1.0f,0.0f, 1.0f},
        {0.0f,1.0f,1.0f, 1.0f},
        {1.0f,0.0f,1.0f, 1.0f},
        {0.5f,0.5f,0.0f, 1.0f},
        {0.0f,0.5f,0.5f, 1.0f},
        {0.5f,0.0f,0.5f, 1.0f},
        {1.0f,0.5f,0.0f, 1.0f},
        {0.0f,0.5f,1.0f, 1.0f},
        {1.0f,0.0f,0.5f, 1.0f},
    };
};
}
