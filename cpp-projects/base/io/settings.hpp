
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

// std
#include <span>

// local
#include "io_enum.hpp"

// thirdparty
#include "json_fwd.hpp"

namespace tool::io {

using namespace std::literals::string_view_literals;

struct Settings{

    SettingsType sType      = SettingsType::Undefined;
    SettingsVersion version = SettingsVersion::Undefined;

    constexpr auto type() const noexcept    -> SettingsType {return sType;};
    auto type_description() const noexcept  -> std::string_view {return settings_name(sType);}
    virtual ~Settings(){}

    // json
    // # save to
    virtual auto save_to_json_binary_file(const std::string &filePath = "./settings.bjson") -> bool;
    virtual auto save_to_json_str_file(const std::string &filePath = "./settings.json") -> bool;    
    // # load from
    auto load_from_file(const std::string &filePath) -> bool;
    // # convert
    virtual auto convert_to_json() const -> nlohmann::json;
    auto convert_to_json_str()      const -> std::string;
    auto convert_to_json_binary()   const  -> std::vector<std::uint8_t>;
    // # init
    virtual auto init_from_json(const nlohmann::json &json) -> void;
    auto init_from_json_binary(std::span<const std::uint8_t> jsonData) -> bool;
    auto init_from_json_str(std::string_view jsonStr) -> bool;

    static auto save_multi_to_json_str_file(std::span<Settings*> settingsA, const std::string &filePath = "./settings.json") -> bool;
    static auto load_multi_from_file(std::span<Settings*> settingsA, const std::string &filePath) -> bool;

protected:

    virtual auto load_from_json_binary_file(const std::string &filePath) -> bool;
    virtual auto load_from_json_str_file(const std::string &filePath) -> bool;
    static auto load_multi_from_json_txt_file(std::span<Settings*> settingsA, const std::string &filePath) -> bool;

    // legacy
    virtual auto init_from_text(std::string_view &text) -> void;
    virtual auto init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void;
    static auto init_from_text_file(std::span<Settings*> settingsA, const std::string &filePath) -> bool;
    static auto init_from_binary_file(std::span<Settings*> settingsA, const std::string &filePath) -> bool;

};

}
