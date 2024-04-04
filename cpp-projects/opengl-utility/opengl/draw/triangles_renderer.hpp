
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
#include "geometry/triangle.hpp"
#include "graphics/animation/bones.hpp"
#include "graphics/color/rgb.hpp"

// local
#include "vao_renderer.hpp"


namespace tool::gl{

class TrianglesRenderer : public VAORenderer {
public:

    ~TrianglesRenderer(){
        TrianglesRenderer::clean();
    }

    auto initialize(bool hasNormals, bool hasTextCoords, bool hasTangents = false, bool hasBones = false, bool hasColors = false) -> void;
    auto clean() -> void override;
    auto load_data(
        std::span<const geo::Pt3<GLuint>> indices,
        std::span<const geo::Pt3f> vertices,
        std::span<const geo::Pt3f> normals   = {},
        std::span<const geo::Pt2f> texCoords = {},
        std::span<const geo::Pt4f> tangents  = {},
        std::span<const graphics::BoneData> bones = {},
        std::span<const ColorRGBA32> colors = {}) -> bool;
    auto update_data(
        std::span<const geo::Pt3<GLuint>> indices,      size_t indicesOffset,
        std::span<const geo::Pt3f> vertices,            size_t verticesOffset   = 0,
        std::span<const geo::Pt3f> normals   = {},      size_t normalsOffset    = 0,
        std::span<const geo::Pt2f> texCoords = {},      size_t textCoordsOffset = 0,
        std::span<const geo::Pt4f> tangents  = {},      size_t tangentsOffset   = 0,
        std::span<const graphics::BoneData> bones = {}, size_t bonesoffset      = 0,
        std::span<const ColorRGBA32> colors = {},       size_t colorsOffset     = 0) -> bool;

    // binding ids
    GLuint positionBindingId    = 0;
    GLuint normalBindingId      = 1;
    GLuint texCoordsBindingId   = 2;
    GLuint tangentsBindingId    = 3;
    // ... bones
    GLuint colorBindingId    = 6;

    // locations
    GLuint positionLoc  = 0;
    GLuint normalLoc    = 1;
    GLuint texCoordsLoc = 2;
    GLuint tangentsLoc  = 3;
    // ... bones
    GLuint colorLoc     = 6;

    // usages
    GLbitfield indicesBufferUsage = 0;
    GLbitfield positionBufferUsage = 0;
    GLbitfield normalBufferUsage = 0;
    GLbitfield texCoordsBufferUsage = 0;
    GLbitfield tangentsBufferUsage = 0;
    GLbitfield bonesBufferUsage = 0;
    GLbitfield colorBufferUsage = 0;

    // ### TO REMOVE
    auto init_and_load_3d_mesh(
        std::span<const geo::Pt3<GLuint>> indices,
        std::span<const geo::Pt3f> points,
        std::span<const geo::Pt3f> normals   = {},
        std::span<const geo::Pt2f> texCoords = {},
        std::span<const geo::Pt4f> tangents  = {},
        std::span<const graphics::BoneData> bones = {},
        std::span<const ColorRGBA32> colors = {}
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
    bool hasTexCoord = false;
    bool hasTangents = false;
    bool hasNormals  = false;
    bool hasBones    = false;
    bool hasColors   = false;
    // ###

protected:

    auto generate_bo() -> void;
    auto vertex_array_vertex_buffer() -> void;
    auto vertex_array_attrib_format() -> void;
    auto enable_vertex_array_attrib() -> void;
    auto vertex_array_attrib_binding() -> void;

    bool m_hasColors    = false;
    bool m_hasNormals   = false;
    bool m_hasTexCoord  = false;
    bool m_hasTangents  = false;
    bool m_hasBones     = false;

    VBO indicesB;
    VBO pointsB;
    VBO normalsB;
    VBO tangentsB;
    VBO colorsB;
    VBO texCoordsB;
    VBO bonesB;    
};


}
