
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
#include <vector>

// base
#include "geometry/mesh.hpp"

// local
#include "opengl/buffer/vertex_buffer_object.hpp"
#include "opengl/buffer/element_buffer_object.hpp"
#include "opengl/vao.hpp"

namespace tool::gl{

class GeometryData{
public:

    virtual ~GeometryData(){}

    VAO vao;
    VBO pointsB;
    GLsizei nIndices = 0;

    virtual auto render() const -> void = 0;
    virtual auto render_adjacency() const -> void {}
    virtual auto render_patches() const -> void {}
    virtual auto render_instances(int count, int baseId = 0) const -> void  {
        static_cast<void>(count);
        static_cast<void>(baseId);

    }

    virtual auto clean() -> void = 0;

protected:
    bool buffersInitialized = false;   
};


class PointMeshData : public GeometryData{
public:

    auto init_buffers(
        GLuint size,
        const geo::Pt3f *points,
        const geo::Pt3f *colors = nullptr,
        const geo::Pt3f *normals = nullptr
    ) -> void;

    auto init_buffers(
        GLuint size,
        const geo::Pt2f *points,
        const geo::Pt3f *colors = nullptr,
        const geo::Pt2f *normals = nullptr
    ) -> void;

    auto init_buffers(
        GLuint size,
        const geo::Pt3<int> *voxels,
        const geo::Pt3f *colors = nullptr
    ) -> void;

    auto render() const -> void override;
    auto render_patches() const -> void override;
    auto clean() -> void override;

protected:

    VBO colorsB;
    VBO normalsB;
};


class LineMeshData : public GeometryData{
public:

    void init_buffers(
        std::vector<GLuint>  *indices,
        std::vector<GLfloat> *points,
        std::vector<GLfloat> *colors = nullptr
    );

    void render() const override;
    void clean() override;

protected:

    VBO colorsB;
    EBO indicesB;
};

class TriangleMeshData : public GeometryData {
public:

    void init_buffers(
        std::vector<GLuint>  *indices,
        std::vector<GLfloat> *points,
        std::vector<GLfloat> *normals   = nullptr,
        std::vector<GLfloat> *texCoords = nullptr,
        std::vector<GLfloat> *tangents  = nullptr,
        std::vector<GLfloat> *colors    = nullptr
    );

    void init_buffers(
        std::vector<geo::TriIds> *indices,
        std::vector<geo::Pt3f> *points,
        std::vector<geo::Pt3f> *normals   = nullptr,
        std::vector<geo::Pt2f> *texCoords = nullptr,
        std::vector<geo::Pt4f> *tangents  = nullptr,
        std::vector<geo::BoneData> *bones = nullptr,
        std::vector<geo::Pt4f> *colors    = nullptr
    );

    void render() const override;
    void render_adjacency() const override;
    void clean() override;

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
