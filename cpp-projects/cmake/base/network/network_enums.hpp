

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
    ipv4= 0, ipv6, unknow
};

enum class Command : std::int8_t{
    quit = 0,
    shutdown,
    restart,
    disconnect
};

enum class FeedbackType : std::int8_t{
    message_received = 0,
    timeout,
    disconnect,
    quit,
    shutdown,
    restart,
    ping,
    undefined,
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
    {FeedbackType::ping,                "ping"sv},
    {FeedbackType::undefined,           "undefined"sv},
}};

[[nodiscard]] [[maybe_unused]] static constexpr auto to_string(FeedbackType m) -> Name{
    return feedbackTypes.at<0,1>(m);
}

[[nodiscard]] [[maybe_unused]] static constexpr auto to_i8(FeedbackType m) -> std::int8_t{
    return static_cast<std::int8_t>(m);
}



}
