
/*******************************************************************************
** Toolset-3d-engine                                                          **
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

// base
#include "geometry/point2.hpp"

// opengl-utility
#include "opengl/buffer/texture_buffer_object.hpp"

namespace tool::graphics {


class ImGuiTextureUiDrawer{
public:

    auto init(gl::TBO *texture, bool invert = false) -> void;
    auto draw_child(const std::string &windowName, geo::Pt2f sizeWindow, std::string_view topTitle = {}, std::string_view infos = {}) -> void;
    auto draw_at_position(const geo::Pt2f &screenPos, const geo::Pt2f &sizeTexture, std::optional<std::string> text = std::nullopt) -> void;

    geo::Pt2<int> hoveringPixel = {-1,-1};
    std::array<bool, 5> mouseButtonsPressed;

private:
    bool m_invert = false;
    gl::TBO *m_texture = nullptr;
    std::string m_infos;
};
}
