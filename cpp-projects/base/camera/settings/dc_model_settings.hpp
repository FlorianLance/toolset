
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
#include "geometry/matrix4.hpp"
#include "files/text_settings.hpp"

namespace tool::camera {
struct DCModelSettings : files::TextSettings{

    geo::Mat4f transformation = geo::Mat4f::identity();

    // local
    geo::Vec3f rotation = {};
    geo::Vec3f translation = {};
    geo::Vec3f scaling = {1.f,1.f,1.f};

    auto compute_full_transformation() const -> geo::Mat4f{
        return geo::transform(scaling, rotation, translation) * transformation;
    }

    // i/o
    auto init_from_text(const std::string &text) -> void override;
    auto convert_to_text() const -> std::string override;
    auto type() const noexcept  -> std::int32_t override {return static_cast<std::int32_t>(tool::files::FileSettingsType::Model);};
};
}
