
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
#include "settings.hpp"
#include "utility/vector.hpp"

namespace tool::files {

struct SubBinarySettings{
    virtual auto init_from_data(std::int8_t *data) -> void = 0;
    virtual auto convert_to_data(std::int8_t *data) const -> void = 0;
    virtual auto total_data_size() const noexcept-> size_t = 0;
};

struct BinaryFileSettings : public BaseSettings{

    auto init_from_file(const std::string &filePath) -> bool{
        return init_from_binary_file(filePath);
    }

    auto save_to_file(const std::string &filePath) const -> bool{
        return save_to_binary_file(filePath);
    }

    static auto save_to_file(const std::vector<BinaryFileSettings*> &settingsA, const std::string &filePath) -> bool{
        return BaseSettings::save_to_binary_file(convert<BaseSettings>(settingsA), filePath);
    }

    static auto init_from_file(std::vector<BinaryFileSettings*> &settingsA, const std::string &filePath) -> bool{
        auto s = convert<BaseSettings>(settingsA);
        return BaseSettings::init_from_binary_file(s, filePath);
    }

private:
    virtual auto init_from_text(const std::string &) -> void override{}
    virtual auto convert_to_text() const -> std::string override{return {};}
};

struct BinarySettings : public BaseSettings{
private:
    virtual auto init_from_text(const std::string &) -> void override{}
    virtual auto convert_to_text() const -> std::string override{return {};}
};

}



