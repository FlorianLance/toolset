
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
#include "geometry/point2.hpp"
#include "geometry/point3.hpp"

// local
#include "vao_renderer.hpp"

namespace tool::gl{

class PointsMeshVAO : public VAORenderer{
public:

    ~PointsMeshVAO(){
        PointsMeshVAO::clean();
    }

    // TODO: test init/load
    // init
    auto init_3d_points(
        size_t size,
        bool hasColors,
        bool hasNormals
    ) -> void;

    // load
    auto load_3d_points(
        std::span<const geo::Pt3f> points,
        std::span<const geo::Pt3f> colors,
        std::span<const geo::Pt3f> normals
    ) -> void;

    // init and load
    auto init_and_load_2d_points(
        std::span<const geo::Pt2f> points,
        std::span<const geo::Pt3f> colors,
        std::span<const geo::Pt2f> normals
    ) -> void;

    auto init_and_load_3d_points(
        std::span<const geo::Pt3f> points,
        std::span<const geo::Pt3f> colors,
        std::span<const geo::Pt3f> normals
    ) -> void;

    auto init_and_load_3d_voxels(
        std::span<const geo::Pt3<int>> voxels,
        std::span<const geo::Pt3f> colors
    ) -> void;

    auto render() const -> void override;
    auto render_patches() const -> void override;
    auto clean() -> void override;

    GLuint positionBindingId = 0;
    GLuint colorBindingId    = 1;
    GLuint normalBindingId   = 2;

    GLuint positionLoc = 0;
    GLuint colorLoc    = 1;
    GLuint normalLoc   = 2;

    GLbitfield positionBufferUsage = 0;
    GLbitfield colorBufferUsage = 0;
    GLbitfield normalBufferUsage = 0;

protected:

    auto generate_vbo() -> void;

    auto vertex_array_vertex_buffer_for_2d_points() -> void;
    auto vertex_array_vertex_buffer_for_3d_points() -> void;
    auto vertex_array_vertex_buffer_for_3d_voxels() -> void;

    auto vertex_array_attrib_format_for_2d_points() -> void;
    auto vertex_array_attrib_format_for_3d_points() -> void;
    auto vertex_array_attrib_format_for_3d_voxels() -> void;

    auto enable_vertex_array_attrib() -> void;
    auto vertex_array_attrib_binding() -> void;

    bool m_hasColors  = false;
    bool m_hasNormals = false;

    VBO colorsB;
    VBO normalsB;

};
}
