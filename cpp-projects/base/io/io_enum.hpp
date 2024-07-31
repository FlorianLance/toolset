
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
#include "utility/tuple_array.hpp"

namespace tool::io {

using namespace std::literals::string_view_literals;

enum class SettingsVersion : std::uint16_t{
    v1_0=0,
    v1_9,
    LastVersion = v1_9,
    Undefined,
    SizeEnum
};

enum class SettingsType : std::int32_t{
    Udp_server, Dc_client_connection, Udp_connection, Remote_server,
    Filters, Color, Model, Delay,
    Device, Device_config, Device_data, Device_actions,
    Undefined,
    SizeEnum
};
using TDCSettings = std::tuple<
    SettingsType,               std::string_view>;
static constexpr TupleArray<SettingsType::SizeEnum, TDCSettings> settings = {{
    TDCSettings
    {SettingsType::Udp_server,      "UDP server"sv},
    {SettingsType::Dc_client_connection, "DC client connection"sv},
    {SettingsType::Udp_connection,  "UDP connection"sv},
    {SettingsType::Remote_server,   "remote server"sv},
    {SettingsType::Filters,         "filters"sv},
    {SettingsType::Color,           "color"sv},
    {SettingsType::Model,           "model"sv},
    {SettingsType::Delay,           "delay"sv},
    {SettingsType::Device,          "device"sv},
    {SettingsType::Device_config,   "device config"sv},
    {SettingsType::Device_data,     "device data"sv},
    {SettingsType::Device_actions,  "device actions"sv},
    {SettingsType::Undefined,       "undefined"sv},
}};

[[maybe_unused]] static constexpr auto settings_name(SettingsType s) -> std::string_view{
    return settings.at<0,1>(s);
}

}
