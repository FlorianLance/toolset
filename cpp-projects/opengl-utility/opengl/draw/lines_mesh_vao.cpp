


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

auto LinesMeshVAO::generate_bo() -> void{

    pointsB.generate();
    if(m_hasColors){
        colorsB.generate();
    }
    indicesB.generate();
}

auto LinesMeshVAO::vertex_array_vertex_buffer_for_3d_lines() -> void{

    GL::vertex_array_vertex_buffer(
        vao.id(),
        positionBindingId,
        pointsB.id(),
        0,
        sizeof(GLfloat)*3
    );
    if(m_hasColors){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            colorBindingId,
            colorsB.id(),
            0,
            sizeof(GLfloat)*3
        );
    }
}

auto LinesMeshVAO::vertex_array_attrib_format_for_3d_lines() -> void{

    GL::vertex_array_attrib_format(
        vao.id(),
        positionBindingId,
        3,
        GL_FLOAT,
        GL_FALSE,
        0
    );
    if(m_hasColors){
        GL::vertex_array_attrib_format(
            vao.id(),
            colorBindingId,
            3,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }
}

auto LinesMeshVAO::enable_vertex_array_attrib() -> void{

    GL::enable_vertex_array_attrib(
        vao.id(),
        positionBindingId
    );
    if(m_hasColors){
        GL::enable_vertex_array_attrib(
            vao.id(),
            colorBindingId
        );
    }
}

auto LinesMeshVAO::vertex_array_attrib_binding() -> void{

    GL::vertex_array_attrib_binding(
        vao.id(),
        positionLoc,
        positionBindingId
    );
    if(m_hasColors){
        GL::vertex_array_attrib_binding(
            vao.id(),
            colorLoc,
            colorBindingId
        );
    }
}


auto LinesMeshVAO::init_data(size_t nbIndices, size_t nbPoints, bool hasColors) -> void{

    if(nbIndices == 0){
        Logger::error("[LinesMeshVAO::init_data] No indices.\n");
        return;
    }
    if(nbPoints == 0){
        Logger::error("[LinesMeshVAO::init_data] No points.\n");
        return;
    }

    if(buffersInitialized){
        clean();
    }

    m_hasColors  = hasColors;

    vao.generate();
    generate_bo();

    vao.bind();
    indicesB.bind();

    const GLfloat *pointsData = nullptr;
    pointsB.load_data(
        pointsData,
        static_cast<GLsizeiptr>(nbPoints*3*sizeof(GLfloat)),
        positionBufferUsage
    );
    if(m_hasColors){
        const GLfloat *colorsData = nullptr;
        colorsB.load_data(
            colorsData,
            static_cast<GLsizeiptr>(nbPoints*3*sizeof(GLfloat)),
            colorBufferUsage
        );
    }
    const GLuint *indicesData = nullptr;
    indicesB.load_data(
        indicesData,
        GLsizeiptr(nbIndices*sizeof(std::uint32_t)),
        indicesBufferUsage
    );

    vertex_array_vertex_buffer_for_3d_lines();
    vertex_array_attrib_format_for_3d_lines();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();
    
    nIndicesAllocated = static_cast<GLsizei>(nbIndices);
    buffersInitialized = true;

    VAO::unbind();
    indicesB.unbind();
}

auto LinesMeshVAO::update_indices(std::span<const GLuint> indices, GLintptr offset) -> void{

    if(!buffersInitialized){
        Logger::error("[LinesMeshVAO::update_indices] Buffer not initialized.\n");
        return;
    }

    indicesB.update_data(
        indices.data(),
        static_cast<GLsizeiptr>(indices.size()*sizeof(std::uint32_t)),
        offset
    );
}

auto LinesMeshVAO::update_data(std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors, GLintptr offset) -> void{

    if(!buffersInitialized){
        Logger::error("[LinesMeshVAO::update_data] Buffer not initialized.\n");
        return;
    }

    pointsB.update_data(
        reinterpret_cast<const GLfloat*>(points.data()),
        static_cast<GLsizeiptr>(points.size()*3*sizeof(GLfloat)),
        offset
    );

    if(m_hasColors){
        colorsB.update_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat)),
            offset
        );
    }
}

auto LinesMeshVAO::init_and_load_data(std::span<const GLuint> indices, std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors) -> void {

    if(indices.empty()){
        Logger::error("[LinesMesh::init_and_load_3d_lines] No indices.\n");
        return;
    }

    if(points.empty()){
        Logger::error("[LinesMesh::init_and_load_3d_lines] No points.\n");
        return;
    }

    if(buffersInitialized){
        clean();
    }

    m_hasColors  = colors.size() == points.size();

    vao.generate();
    generate_bo();

    vao.bind();
    indicesB.bind();

    indicesB.load_data(
        indices.data(),
        GLsizeiptr(indices.size()*sizeof(std::uint32_t)),
        indicesBufferUsage
    );

    pointsB.load_data(
        reinterpret_cast<const GLfloat*>(points.data()),
        static_cast<GLsizeiptr>(points.size()*3*sizeof(GLfloat)),
        positionBufferUsage
    );

    if(m_hasColors){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat)),
            colorBufferUsage
        );
    }

    vertex_array_vertex_buffer_for_3d_lines();
    vertex_array_attrib_format_for_3d_lines();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();
    
    nIndicesAllocated = static_cast<GLsizei>(indices.size());    
    buffersInitialized = true;

    VAO::unbind();
    indicesB.unbind();
}

auto LinesMeshVAO::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_lines_with_ebo(nIndicesAllocated);
    VAO::unbind();
}

auto LinesMeshVAO::clean() -> void{
    vao.clean();
    pointsB.clean();
    indicesB.clean();
    colorsB.clean();
    buffersInitialized = false;
    m_hasColors = false;
}

