
/*******************************************************************************
** Toolset-opengl-utility                                                     **
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
#include <span>

// base
#include "geometry/point3.hpp"

// local
#include "vao_renderer.hpp"

namespace tool::gl{

class VoxelsRenderer : public VAORenderer{
public:
    ~VoxelsRenderer(){
        VoxelsRenderer::clean();
    }

    auto initialize(bool hasColors) -> void;
    auto clean() -> void override;
    auto load_data(
        std::span<const geo::Pt3<int>> vertices,
        std::span<const geo::Pt3f> colors
    ) -> bool;
    auto update_data(
        std::span<const geo::Pt3<int>> vertices,    size_t verticesOffset = 0,
        std::span<const geo::Pt3f> colors  = {},    size_t colorsOffset   = 0
    ) -> bool;

    GLuint positionBindingId = 0;
    GLuint colorBindingId    = 1;

    GLuint positionLoc = 0;
    GLuint colorLoc    = 1;

    GLbitfield positionBufferUsage = 0;
    GLbitfield colorBufferUsage = 0;

protected:

    auto generate_vbo() -> void;
    auto vertex_array_vertex_buffer() -> void;
    auto vertex_array_attrib_format() -> void;
    auto enable_vertex_array_attrib() -> void;
    auto vertex_array_attrib_binding() -> void;

    bool m_hasColors  = false;

    VBO colorsB;
};
}
