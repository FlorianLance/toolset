
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
#include "network/network_utility.hpp"
#include "files/text_settings.hpp"
#include "camera/kinect4/k4_types.hpp"

namespace tool {

struct K4ServerNetworkSettings : files::TextSettings{

    auto initialize() -> bool;

    std::vector<network::Interface> interfaces = {};
    std::vector<network::ReadSendNetworkInfos> clientsInfo;

private:

    auto init_from_text(const std::string &text) -> void override;
    auto convert_to_text() const -> std::string override;

    auto type() const noexcept  -> std::int32_t override {return static_cast<std::int32_t>(camera::SettingsType::Network);};
    auto file_description() const noexcept -> std::string_view override {return settings_name(static_cast<camera::SettingsType>(type()));}
};
}
