
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

class LinesMeshVAO : public VAORenderer{
public:

    ~LinesMeshVAO(){
        LinesMeshVAO::clean();
    }

    auto init_data(size_t nbIndices, size_t nbPoints, bool hasColors) -> void;
    auto init_and_load_data(
        std::span<const GLuint> indices,
        std::span<const geo::Pt3f> points,
        std::span<const geo::Pt3f> colors
    ) -> void;

    auto update_indices(std::span<const GLuint> indices, GLintptr offset = 0) -> void;
    auto update_data(std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors, GLintptr offset = 0) -> void;

    auto clean() -> void override;

    // TO REMOVE
    auto render() const -> void override;    

    GLuint positionBindingId = 0;
    GLuint colorBindingId    = 1;

    GLuint positionLoc = 0;
    GLuint colorLoc    = 1;

    GLbitfield indicesBufferUsage = 0;
    GLbitfield positionBufferUsage = 0;
    GLbitfield colorBufferUsage = 0;

protected:

    auto generate_bo() -> void;
    auto vertex_array_vertex_buffer_for_3d_lines() -> void;
    auto vertex_array_attrib_format_for_3d_lines() -> void;
    auto enable_vertex_array_attrib() -> void;
    auto vertex_array_attrib_binding() -> void;

    bool m_hasColors  = false;

    EBO indicesB;
    VBO colorsB;
};


}
