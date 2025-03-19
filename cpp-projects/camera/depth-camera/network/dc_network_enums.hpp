
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
#include "network/network_enums.hpp"

namespace tool::net{

using namespace std::literals::string_view_literals;
using Name = std::string_view;

enum class DCMessageType : std::int8_t {
    undefined = 0,
    init_server_client_connection,
    update_device_settings,
    update_color_settings,
    update_filters_settings,
    update_misc_settings,
    data_frame,
    command,
    feedback,
    synchro,
    ping,
    SizeEnum
};

using TDCMessageTypes = std::tuple<
    DCMessageType,                                Name>;
static constexpr TupleArray<DCMessageType::SizeEnum, TDCMessageTypes> DCMessageTypes = {{
    TDCMessageTypes
    {DCMessageType::init_server_client_connection,  "init_server_client_connection"sv},
    {DCMessageType::update_device_settings,         "update_device_settings"sv},
    {DCMessageType::update_color_settings,          "update_color_settings"sv},
    {DCMessageType::update_filters_settings,        "update_filters_settings"sv},
    {DCMessageType::update_misc_settings,           "update_misc_settings"sv},
    {DCMessageType::data_frame,                     "data_frame"sv},
    {DCMessageType::command,                        "command"sv},
    {DCMessageType::feedback,                       "feedback"sv},
    {DCMessageType::synchro,                        "synchro"sv},
    {DCMessageType::ping,                           "ping"sv},
}};

[[nodiscard]] [[maybe_unused]] static constexpr auto to_string(DCMessageType m) -> Name{
    return DCMessageTypes.at<0,1>(m);
}

}
