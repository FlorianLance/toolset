
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
#include "utility/settings.hpp"
#include "dc_device_data_settings.hpp"

namespace tool::cam {

struct DCDataSettings : Settings{

    [[nodiscard]] constexpr auto server_capture_color()       const noexcept -> bool{return server.capture.color;}
    [[nodiscard]] constexpr auto capture_depth()              const noexcept -> bool{return server.capture.depth;}
    [[nodiscard]] constexpr auto capture_infra()              const noexcept -> bool{return server.capture.infra;}
    [[nodiscard]] constexpr auto capture_body_tracking()      const noexcept -> bool{return server.capture.bodyTracking;}
    [[nodiscard]] constexpr auto capture_audio()              const noexcept -> bool{return server.capture.audio;}
    [[nodiscard]] constexpr auto capture_imu()                const noexcept -> bool{return server.capture.imu;}

    DCDeviceDataSettings server;
    DCFrameGenerationSettings clientGeneration;

    DCDataSettings(){
        sType   = SettingsType::Device_data;
        version = SettingsVersion::LastVersion;
    }
    DCDataSettings(std::span<const std::uint8_t> jsonBinary){
        DCDataSettings::init_from_json_binary(jsonBinary);
    }

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    auto apply_remote_profile() -> void;
    auto apply_local_profile() -> void;

    // legacy
    auto init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void override;
};

// struct DCClientDataSettings{
//     DCDeviceDataSettings server;
//     DCFrameGenerationSettings clientGeneration;
// };

// struct DCServerDataSettings{
//     DCDeviceDataSettings server;
// };


}
