
/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include "dc_config_settings.hpp"
#include "dc_data_settings.hpp"

namespace tool::cam {

struct DCDeviceSettings : Settings{

    DCConfigSettings configS;
    DCDataSettings dataS;

    // runtime
    // ...
    
    static auto default_init_for_server() -> DCDeviceSettings;
    static auto default_init_for_client() -> DCDeviceSettings;

    DCDeviceSettings(){
        sType   = SettingsType::Device;
        version = SettingsVersion::LastVersion;
    }
    DCDeviceSettings(std::span<const std::uint8_t> jsonBinary){
        DCDeviceSettings::init_from_json_binary(jsonBinary);
    }

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    // update
    auto update_with_device_id(size_t order, size_t id) -> void;

    // legacy
    auto init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void override;
};
}
