
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

#include "points_mesh.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::geo;
using namespace tool::gl;

auto PointsMesh::init_3d_points(size_t size, bool hasColors, bool hasNormals) -> void{

    if(size == 0){
        Logger::error("[PointsMesh::init_3d_points] No points.\n");
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

auto PointsMesh::load_3d_points(std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void{

    if(!buffersInitialized){
        Logger::error("[PointsMesh::load_3d_points] Buffer not initialized.\n");
        return;
    }

    // check original buffer size
    // ...

}

auto PointsMesh::init_and_load_3d_points(std::span<const Pt3f> points, std::span<const Pt3f> colors, std::span<const Pt3f> normals) -> void{

    if(points.empty()){
        Logger::error("[PointsMesh::init_and_load_3d_points] No points.\n");
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
    if(hasColors){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat))
        );
    }
    if(hasNormals){
        normalsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat))
        );
    }


    GLuint positionBindingId = 0;
    GLuint colorBindingId    = 1;
    GLuint normalBindingId   = 2;

    GLuint positionLoc = 0;
    GLuint colorLoc    = 1;
    GLuint normalLoc   = 2;

    // Bind vbo to buffer bind point
    GL::vertex_array_vertex_buffer(
        vao.id(),
        positionBindingId,
        pointsB.id(),
        0,
        sizeof(GLfloat)*3
    );
    if(hasColors){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            colorBindingId,
            colorsB.id(),
            0,
            sizeof(GLfloat)*3
        );
    }
    if(hasNormals){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            normalBindingId,
            normalsB.id(),
            0,
            sizeof(GLfloat)*3
        );
    }

    // Specify the organization of vertex arrays
    GL::vertex_array_attrib_format(
        vao.id(),
        positionBindingId,
        3,
        GL_FLOAT,
        GL_FALSE,
        0
    );
    if(hasColors){
        GL::vertex_array_attrib_format(
            vao.id(),
            colorBindingId,
            3,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }
    if(hasNormals){
        GL::vertex_array_attrib_format(
            vao.id(),
            normalBindingId,
            3,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }

    // Enable generic vertex attribute array
    GL::enable_vertex_array_attrib(
        vao.id(),
        positionBindingId
    );
    if(hasColors){
        GL::enable_vertex_array_attrib(
            vao.id(),
            colorBindingId
        );
    }
    if(hasNormals){
        GL::enable_vertex_array_attrib(
            vao.id(),
            normalBindingId
        );
    }

    // Associate a vertex attribute and a vertex buffer binding for a vertex array object
    GL::vertex_array_attrib_binding(
        vao.id(),
        positionLoc,
        positionBindingId
    );
    if(hasColors){
        GL::vertex_array_attrib_binding(
            vao.id(),
            colorLoc,
            colorBindingId
        );
    }
    if(hasNormals){
        GL::vertex_array_attrib_binding(
            vao.id(),
            normalLoc,
            normalBindingId
        );
    }

    nIndices = static_cast<GLsizei>(points.size());
    buffersInitialized = true;
    VAO::unbind();
}


auto PointsMesh::init_and_load_2d_points(std::span<const Pt2f> points, std::span<const Pt3f> colors, std::span<const Pt2f> normals) -> void {

    if(points.empty()){
        Logger::error("[PointsMesh::init_and_load_2d_points] No points.\n");
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

auto PointsMesh::init_and_load_3d_voxels(std::span<const Pt3<int>> voxels, std::span<const Pt3f> colors) -> void{

    if(voxels.empty()){
        Logger::error("[PointsMesh::init_and_load_3d_voxels] No voxels.\n");
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

auto PointsMesh::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    GL::draw_arrays_instance_base_instance(GL_POINTS, 0, nIndices, 1, 0);
    VAO::unbind();
}

auto PointsMesh::render_patches() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    GL::draw_arrays_instance_base_instance(GL_PATCHES, 0, nIndices, 1, 0);
    VAO::unbind();
}

auto PointsMesh::clean() -> void{
    vao.clean();
    pointsB.clean();
    colorsB.clean();
    normalsB.clean();
    buffersInitialized = false;
}
