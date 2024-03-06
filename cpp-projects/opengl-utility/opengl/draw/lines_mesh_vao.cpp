


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

#include "lines_mesh_vao.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "opengl/gl_draw.hpp"

using namespace tool::geo;
using namespace tool::gl;

auto LinesMeshVAO::init_and_load_3d_lines(std::span<const GLuint> indices, std::span<const geo::Pt3f> points, std::span<const geo::Pt4f> colors) -> void {

    if(indices.empty()){
        Logger::error("[LinesMesh::init_and_load_3d_lines] No indices.\n");
        return;
    }

    if(points.empty()){
        Logger::error("[LinesMesh::init_and_load_3d_lines] No points.\n");
        return;
    }

    if(indices.size() > points.size()*3){
        Logger::error("[LinesMesh::init_and_load_3d_lines] Invalid points size.\n");
        return;
    }


    if(buffersInitialized){
        clean();
    }

    bool hasColors  = colors.size() == points.size();

    // generate vao
    vao.generate();

    // generate vbo
    pointsB.generate();
    if(hasColors){
        colorsB.generate();
    }

    // generate ebo
    indicesB.generate();


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
            static_cast<GLsizeiptr>(colors.size()*4*sizeof(GLfloat))
        );
        colorsB.dsa_attrib(
            vao.id(),
            1,
            4,
            sizeof(Pt4f),
            GL_FLOAT,
            0,
            1
        );
    }

    // load ebo
    vao.bind();
    indicesB.bind();
    indicesB.load_data(
        indices.data(),
        GLsizeiptr(indices.size()*sizeof(std::uint32_t))
    );

    nIndices = static_cast<GLsizei>(indices.size());
    VAO::unbind();
    buffersInitialized = true;
}

auto LinesMeshVAO::init_and_load_3d_lines(std::vector<GLuint> *indices, std::vector<GLfloat> *points, std::vector<GLfloat> *colors) -> void{

    // check inputs
    if(indices == nullptr || points == nullptr){
        Logger::error("[LinesMesh::init_and_load_3d_lines] error, no indices or points buffers.\n");
        return;
    }

    if(indices->size() == 0 || points->size() == 0){
        Logger::error("[LinesMesh::init_and_load_3d_lines] error, empty buffers.\n");
        return;
    }

    if(colors != nullptr){
        if(points->size()/3 != colors->size()/4){
            Logger::error("[LinesMesh::init_and_load_3d_lines] error, different size for points and colors buffers.\n");
            return;
        }
    }

    // clean buffers
    if(buffersInitialized){
        clean();
    }

    // generate vao
    vao.generate();

    // generate vbo
    pointsB.generate();
    if(colors != nullptr){
        colorsB.generate();
    }

    // generate ebo
    indicesB.generate();


    // load data
    vao.bind();
    indicesB.bind();
    indicesB.load_data(indices->data(), GLsizeiptr(indices->size()*sizeof(std::uint32_t)));

    pointsB.load_data(
        points->data(),
        static_cast<GLsizeiptr>(points->size()*sizeof(GLfloat))
    );
    pointsB.attrib(AttriIndex{0}, AttriSize{3}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});

    if(colors != nullptr){
        colorsB.load_data(
            colors->data(),
            static_cast<GLsizeiptr>(colors->size()*sizeof(GLfloat))
        );
        colorsB.attrib(AttriIndex{1}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }

    nIndices = static_cast<GLsizei>(indices->size());
    VAO::unbind();
    buffersInitialized = true;
}

auto LinesMeshVAO::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_lines_with_ebo(nIndices);
    VAO::unbind();
}

auto LinesMeshVAO::clean() -> void{
    vao.clean();
    pointsB.clean();
    indicesB.clean();
    colorsB.clean();
    buffersInitialized = false;
}
