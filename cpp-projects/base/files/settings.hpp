
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
#include <vector>
#include <string>
#include <optional>

namespace tool::files {

using namespace std::literals::string_view_literals;

struct BaseSettings{

protected:
    virtual auto init_from_binary_file(const std::string &filePath) -> bool;
    virtual auto init_from_text_file(const std::string &filePath) -> bool;

    virtual auto save_to_text_file(const std::string &filePath) const -> bool;
    virtual auto save_to_binary_file(const std::string &filePath) const -> bool;

    virtual auto init_from_text(const std::string &text) -> void = 0;
    virtual auto convert_to_text() const -> std::string = 0;

    virtual auto init_from_data(std::int8_t *data) -> void = 0;
    virtual auto convert_to_data(std::int8_t *data) const -> void = 0;
    virtual auto total_data_size() const noexcept-> size_t = 0;

    virtual auto type() const noexcept -> std::int32_t {return 0;};
    virtual auto file_description() const noexcept -> std::string_view {return "default"sv;}

    static auto save_to_text_file(const std::vector<BaseSettings*> &settingsA, const std::string &filePath) -> bool;
    static auto init_from_text_file(std::vector<BaseSettings*> &settingsA, const std::string &filePath) -> bool;

    static auto save_to_binary_file(const std::vector<BaseSettings*> &settingsA, const std::string &filePath) -> bool;
    static auto init_from_binary_file(std::vector<BaseSettings*> &settingsA, const std::string &filePath) -> bool;

    size_t m_inputFileSize = 0;
};

}
