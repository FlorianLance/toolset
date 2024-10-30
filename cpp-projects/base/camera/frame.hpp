
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

// std
#include <fstream>
#include <span>

// thirdparty
#include "json_fwd.hpp"

namespace tool::cam{

struct Frame{

    std::uint8_t idDevice        = 0;
    std::int32_t idCapture       = 0;
    std::int64_t afterCaptureTS  = 0;
    std::int64_t receivedTS      = 0;

    virtual ~Frame(){}    

    virtual auto init_from_json(const nlohmann::json &json) -> void;
    virtual auto convert_to_json() const -> nlohmann::json;

    auto convert_to_json_str() const -> std::string;
    auto convert_to_json_binary() const  -> std::vector<std::uint8_t>;
    auto init_from_json_binary(std::span<const std::uint8_t> jsonData) -> bool;
    auto init_from_json_str(std::string_view jsonStr) -> bool;

    // legacy
    virtual auto data_size() const noexcept -> size_t;
    virtual auto init_from_file_stream(std::ifstream &file) -> void;
    virtual auto init_from_data(std::span<const std::byte> data, size_t &offset) -> void;
    virtual auto write_to_file_stream(std::ofstream &file) -> void;
    virtual auto write_to_data(std::span<std::byte> data, size_t &offset) -> void;
};
}
