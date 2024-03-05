
/*******************************************************************************
** Toolset-opengl-utility                                                     **
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

// base
#include "geometry/point3.hpp"
#include "geometry/point4.hpp"

// local
#include "vao_renderer.hpp"
#include "opengl/buffer/element_buffer_object.hpp"

namespace tool::gl{

class LinesMesh : public VAORenderer{
public:

    auto init_and_load_3d_lines(
        std::span<const GLuint> indices,
        std::span<const geo::Pt3f> points,
        std::span<const geo::Pt4f> colors
    ) -> void;

    auto init_and_load_3d_lines(
        std::vector<GLuint>  *indices,
        std::vector<GLfloat> *points,
        std::vector<GLfloat> *colors = nullptr
    ) -> void;

    auto render() const -> void override;
    auto clean() -> void override;

protected:

    EBO indicesB;
    VBO colorsB;
};


}
