


/*******************************************************************************
** Toolset-ts-imgui-gl-engine                                                 **
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

// imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"

// local
#include "geometry/point2.hpp"
#include "geometry/point4.hpp"

namespace tool {

[[maybe_unused]] static inline auto to_iv2(geo::Pt2<int> p) -> ImVec2{
    return ImVec2(static_cast<float>(p.x()),static_cast<float>(p.y()));
}
[[maybe_unused]] static inline auto to_iv2(geo::Pt2f p) -> ImVec2{
    return ImVec2(p.x(),p.y());
}
[[maybe_unused]] static inline auto to_iv4(geo::Pt4f p) -> ImVec4{
    return ImVec4(p.x(),p.y(),p.z(),p.w());
}
[[maybe_unused]] static inline auto to_pt2(ImVec2 v) -> geo::Pt2f{
    return {v.x,v.y};
}
[[maybe_unused]] static inline auto to_pt4(ImVec4 v) -> geo::Pt4f{
    return {v.x,v.y,v.z,v.w};
}


}


