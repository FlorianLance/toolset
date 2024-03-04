
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

#include "points_mesh_data.hpp"

using namespace tool::geo;
using namespace tool::gl;

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"


auto PointMeshData::init_3d_points(size_t size, bool hasColors, bool hasNormals) -> void{

    if(size == 0){
        Logger::error("[PointMeshData::init_3d_points] No points.\n");
        return;
    }

    if(buffersInitialized){
        clean();
    }

    // generate vao
    vao.generate();

    // generate vbo
    pointsB.generate();
    if(hasColors){
        colorsB.generate();
    }
    if(hasNormals){
        normalsB.generate();
    }

    // load vbo
    const GLfloat *pointsData = nullptr;
    pointsB.load_data(
        pointsData,
        static_cast<GLsizeiptr>(size*3*sizeof(GLfloat))
    );
    pointsB.dsa_attrib(
        vao.id(),
        0,
        3,
        sizeof(Pt3f),
        GL_FLOAT,
        0,
        0
    );

    if(hasColors){
        const GLfloat *colorsData = nullptr;
        colorsB.load_data(
            colorsData,
            static_cast<GLsizeiptr>(size*3*sizeof(GLfloat))
        );
        colorsB.dsa_attrib(
            vao.id(),
            1,
            3,
            sizeof(Pt3f),
            GL_FLOAT,
            0,
            1
        );
    }

    nIndices = static_cast<GLsizei>(size);
    buffersInitialized = true;
    VAO::unbind();
}

auto PointMeshData::load_3d_points(std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void{

    if(!buffersInitialized){
        Logger::error("[PointMeshData::load_3d_points] Buffer not initialized.\n");
        return;
    }

    // check original buffer size
    // ...

}

auto PointMeshData::init_and_load_3d_points(std::span<const Pt3f> points, std::span<const Pt3f> colors, std::span<const Pt3f> normals) -> void{

    if(points.empty()){
        Logger::error("[PointMeshData::init_and_load_3d_points] No points.\n");
        return;
    }

    if(buffersInitialized){
        clean();
    }

    bool hasColors  = colors.size() == points.size();
    bool hasNormals = normals.size() == points.size();

    // generate vao
    vao.generate();

    // generate vbo
    pointsB.generate();
    if(hasColors){
        colorsB.generate();
    }
    if(hasNormals){
        normalsB.generate();
    }

    // load vbo
    pointsB.load_data(
        reinterpret_cast<const GLfloat*>(points.data()),
        static_cast<GLsizeiptr>(points.size()*3*sizeof(GLfloat))
    );
    pointsB.dsa_attrib(
        vao.id(),
        0,
        3,
        sizeof(Pt3f),
        GL_FLOAT,
        0,
        0
    );

    if(hasColors){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat))
        );
        colorsB.dsa_attrib(
            vao.id(),
            1,
            3,
            sizeof(Pt3f),
            GL_FLOAT,
            0,
            1
        );
    }

    if(hasNormals){
        normalsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat))
        );
        normalsB.dsa_attrib(
            vao.id(),
            2,
            3,
            sizeof(Pt3f),
            GL_FLOAT,
            0* sizeof(float),
            2
        );
    }

    nIndices = static_cast<GLsizei>(points.size());
    buffersInitialized = true;
    VAO::unbind();
}


auto PointMeshData::init_and_load_2d_points(std::span<const Pt2f> points, std::span<const Pt3f> colors, std::span<const Pt2f> normals) -> void {

    if(points.empty()){
        Logger::error("[PointMeshData::init_and_load_2d_points] No points.\n");
        return;
    }

    if(buffersInitialized){
        clean();
    }

    bool hasColors  = colors.size() == points.size();
    bool hasNormals = normals.size() == points.size();

    // generate vao
    vao.generate();

    // generate vbo
    pointsB.generate();
    if(hasColors){
        colorsB.generate();
    }
    if(hasNormals){
        normalsB.generate();
    }

    // load vbo
    pointsB.load_data(
        reinterpret_cast<const GLfloat*>(points.data()),
        static_cast<GLsizeiptr>(points.size()*2*sizeof(GLfloat))
    );
    pointsB.dsa_attrib(
        vao.id(),
        0,
        2,
        sizeof(Pt2f),
        GL_FLOAT,
        0,
        0
    );

    if(hasColors){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat))
        );
        colorsB.dsa_attrib(
            vao.id(),
            1,
            3,
            sizeof(Pt3f),
            GL_FLOAT,
            0,
            1
        );
    }

    if(hasNormals){
        normalsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*2*sizeof(GLfloat))
        );
        normalsB.dsa_attrib(
            vao.id(),
            2,
            2,
            sizeof(Pt2f),
            GL_FLOAT,
            0* sizeof(float),
            2
        );
    }

    nIndices = static_cast<GLsizei>(points.size());
    buffersInitialized = true;
    VAO::unbind();
}

auto PointMeshData::init_and_load_3d_voxels(std::span<const Pt3<int>> voxels, std::span<const Pt3f> colors) -> void{

    if(voxels.empty()){
        Logger::error("[PointMeshData::init_and_load_3d_voxels] No voxels.\n");
        return;
    }

    if(buffersInitialized){
        clean();
    }

    bool hasColors  = colors.size() == voxels.size();

    // generate vao
    vao.generate();

    // generate vbo
    pointsB.generate();
    if(hasColors){
        colorsB.generate();
    }

    // load vbo
    pointsB.load_data(
        reinterpret_cast<const GLint*>(voxels.data()),
        static_cast<GLsizeiptr>(voxels.size()*3*sizeof(GLint))
    );
    pointsB.dsa_attrib(
        vao.id(),
        0,
        3,
        sizeof(Pt3<int>),
        GL_INT,
        0,
        0
    );

    if(hasColors){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat))
        );
        colorsB.dsa_attrib(
            vao.id(),
            1,
            3,
            sizeof(Pt3f),
            GL_FLOAT,
            0,
            1
        );
    }

    nIndices = static_cast<GLsizei>(voxels.size());
    buffersInitialized = true;
    VAO::unbind();
}

auto PointMeshData::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    GL::draw_arrays_instance_base_instance(GL_POINTS, 0, nIndices, 1, 0);
    VAO::unbind();
}

auto PointMeshData::render_patches() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    GL::draw_arrays_instance_base_instance(GL_PATCHES, 0, nIndices, 1, 0);
    VAO::unbind();
}

auto PointMeshData::clean() -> void{
    vao.clean();
    pointsB.clean();
    colorsB.clean();
    normalsB.clean();
    buffersInitialized = false;
}
