
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

#include "dc_model_settings.hpp"

// std
#include <format>

// local
#include "utility/string.hpp"

using namespace std::literals::string_view_literals;
using namespace tool::camera;

DCModelSettings::DCModelSettings(){
    sType   = io::SettingsType::Model;
    version = io::Version::v1_0;
}

auto DCModelSettings::compute_full_transformation() const -> tool::geo::Mat4f{
    return geo::transform(scaling, rotation, translation) * transformation;
}

auto DCModelSettings::init_from_text(std::string_view &text) -> void  {

    io::BaseSettings::init_from_text(text);

    size_t id = 0;
    for(const auto &line : String::split_view(text, "\n"sv)){
        if(line.length() == 0){
            return;
        }
        for(auto value : String::split_view(line, " "sv)){
            transformation.array[id++] = String::to_float(value);
            if(id == 16){
                return;
            }
        }
    }
}

auto DCModelSettings::write_to_text() const -> std::string {
    const auto &t = transformation;
    return std::format("{}{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n",
        BaseSettings::write_to_text(),
        t.at(0),t.at(1),t.at(2),t.at(3),
        t.at(4),t.at(5),t.at(6),t.at(7),
        t.at(8),t.at(9),t.at(10),t.at(11),
        t.at(12),t.at(13),t.at(14),t.at(15)
    );
}
