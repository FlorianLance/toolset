
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
#include "geometry/point4.hpp"
#include "geometry/triangle3.hpp"
#include "graphics/animation/bones.hpp"
#include "graphics/color/rgb.hpp"

// local
#include "vao_renderer.hpp"
#include "opengl/buffer/element_buffer_object.hpp"

namespace tool::gl{

class TriangleMesh : public VAORenderer {
public:

    auto init_and_load_3d_mesh(
        std::span<geo::TriIds> indices,
        std::span<geo::Pt3f> points,
        std::span<geo::Pt3f> normals   = {},
        std::span<geo::Pt2f> texCoords = {},
        std::span<geo::Pt4f> tangents  = {},
        std::span<graphics::BoneData> bones = {},
        std::span<ColorRGBA32> colors = {}
    ) -> void;


    auto init_buffers(
        std::vector<GLuint>  *indices,
        std::vector<GLfloat> *points,
        std::vector<GLfloat> *normals   = nullptr,
        std::vector<GLfloat> *texCoords = nullptr,
        std::vector<GLfloat> *tangents  = nullptr,
        std::vector<GLfloat> *colors    = nullptr
    ) -> void;

    auto init_buffers(
        std::vector<geo::TriIds> *indices,
        std::vector<geo::Pt3f> *points,
        std::vector<geo::Pt3f> *normals   = nullptr,
        std::vector<geo::Pt2f> *texCoords = nullptr,
        std::vector<geo::Pt4f> *tangents  = nullptr,
        std::vector<graphics::BoneData> *bones = nullptr,
        std::vector<ColorRGBA32> *colors    = nullptr
    ) -> void;

    auto render() const -> void override;
    auto render_adjacency() const -> void override;
    auto clean() -> void override;

    bool hasTexCoord = false;
    bool hasTangents = false;
    bool hasNormals  = false;
    bool hasBones    = false;
    bool hasColors   = false;

protected:

    VBO pointsB;
    VBO normalsB;
    VBO tangentsB;
    VBO colorsB;
    VBO texCoordsB;
    VBO bonesB;
    EBO indicesB;

};


}