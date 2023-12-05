
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

namespace tool::files {

using namespace std::literals::string_view_literals;

enum class FileSettingsType : std::int32_t{
    Network, Filters, Color, Device, Model, Delay, Undefined,
    SizeEnum
};
using TDCSettings = std::tuple<
    FileSettingsType,               std::string_view>;
static constexpr TupleArray<FileSettingsType::SizeEnum, TDCSettings> filesSettings = {{
    TDCSettings
    {FileSettingsType::Network,     "network settings"sv},
    {FileSettingsType::Filters,     "filters settings"sv},
    {FileSettingsType::Color,       "color settings"sv},
    {FileSettingsType::Device,      "device settings"sv},
    {FileSettingsType::Model,       "calibration"sv},
    {FileSettingsType::Delay,       "delay"sv},
    {FileSettingsType::Undefined,   "undefined"sv},
}};

[[maybe_unused]] static constexpr auto settings_name(FileSettingsType s) -> std::string_view{
    return filesSettings.at<0,1>(s);
}

}
