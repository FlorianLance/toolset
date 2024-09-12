

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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
#include "dc_client_device_settings.hpp"

namespace tool::cam {

struct DCClientSettings : public io::Settings{

    DCClientSettings();

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    auto add_device(DCClientType connectionType) -> void;
    auto remove_last_device() -> void;
    auto update_connection_settings(DCDeviceConnectionSettings &connectionS) -> void;

    // settings
    size_t clientId = 0;
    DCSceneDisplaySettings sceneDisplayS;
    Buffer<DCClientDeviceSettings> devicesS;
    bool useNormalFilteringSettings = true;

    // runtime
    std::string filePath;
    std::vector<net::Interface> ipv4Interfaces = {};
    std::vector<net::Interface> ipv6Interfaces = {};

};
}
