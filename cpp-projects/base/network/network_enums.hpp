

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

namespace tool::net{

enum class Protocol : std::uint8_t{
    ipv4, ipv6, unknow
};

enum class FeedbackType : std::int8_t{
    message_received = 0,
    timeout,
    disconnect,
    quit,
    shutdown,
    restart,
    update_device_list,
    SizeEnum
};

using namespace std::literals::string_view_literals;
using Name = std::string_view;
using TFeedbackTypes = std::tuple<
    FeedbackType,                       Name>;
static constexpr TupleArray<FeedbackType::SizeEnum, TFeedbackTypes> feedbackTypes = {{
TFeedbackTypes
    {FeedbackType::message_received,    "message_received"sv},
    {FeedbackType::timeout,             "timeout"sv},
    {FeedbackType::disconnect,          "disconnect"sv},
    {FeedbackType::quit,                "quit"sv},
    {FeedbackType::shutdown,            "shutdown"sv},
    {FeedbackType::restart,             "restart"sv},
    {FeedbackType::update_device_list,  "update_device_list"sv},
}};


[[maybe_unused]] static constexpr auto to_string(FeedbackType m) -> Name{
    return feedbackTypes.at<0,1>(m);
}

[[maybe_unused]] static constexpr auto to_i8(FeedbackType m) -> std::int8_t{
    return static_cast<std::int8_t>(m);
}

enum class MessageType : std::int8_t {
    init_network_infos = 0,
    update_device_settings,
    update_color_settings,
    update_filters,
    compressed_frame_data,
    command,
    feedback,
    delay,
    synchro,
    ping,
    undefined,
    // filter_mask
    SizeEnum
};

using TMessageTypes = std::tuple<
    MessageType,                                Name>;
static constexpr TupleArray<MessageType::SizeEnum, TMessageTypes> MessageTypes = {{
    TMessageTypes
    {MessageType::init_network_infos,           "init_network_infos"sv},
    {MessageType::update_device_settings,       "update_device_settings"sv},
    {MessageType::update_color_settings,        "update_color_settings"sv},
    {MessageType::update_filters,               "update_filters"sv},
    {MessageType::compressed_frame_data,        "compressed_frame_data"sv},
    {MessageType::command,                      "command"sv},
    {MessageType::feedback,                     "feedback"sv},
    {MessageType::delay,                        "delay"sv},
    {MessageType::synchro,                      "synchro"sv},
    {MessageType::ping,                         "ping"sv},
}};

[[maybe_unused]] static constexpr auto to_string(MessageType m) -> Name{
    return MessageTypes.at<0,1>(m);
}

[[maybe_unused]] static constexpr auto to_i8(MessageType m) -> std::int8_t{
    return static_cast<std::int8_t>(m);
}

enum class Command : std::int8_t{
    Quit = 0,
    Shutdown,
    Restart,
    Disconnect,
    UpdateDeviceList
};
}
