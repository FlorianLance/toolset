

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

#include "voxels_renderer.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

auto tool::gl::VoxelsRenderer::initialize(bool hasColors) -> void{

    if(m_buffersInitialized){
        clean();
    }

    m_hasColors  = hasColors;

    vao.initialize();
    generate_vbo();

    vertex_array_vertex_buffer();
    vertex_array_attrib_format();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();

    m_buffersInitialized = true;
}

auto tool::gl::VoxelsRenderer::clean() -> void{

    vao.clean();

    pointsB.clean();
    colorsB.clean();

    m_buffersInitialized = false;

    m_nbIndices = 0;
    m_nbVertices = 0;

    m_hasColors = false;

    positionBufferUsage = 0;
    colorBufferUsage = 0;
}

auto tool::gl::VoxelsRenderer::load_data(std::span<const geo::Pt3<int>> vertices, std::span<const geo::Pt3f> colors) -> bool{

    if(!vertices.empty()){
        pointsB.load_data(
            reinterpret_cast<const GLint*>(vertices.data()),
            static_cast<GLsizeiptr>(vertices.size()*3*sizeof(GLint)),
            positionBufferUsage
        );
        m_nbVertices = static_cast<GLsizei>(vertices.size());
        m_nbIndices  = m_nbVertices;
    }

    if(m_nbVertices == 0){
        Logger::error("[VoxelsRenderer::load_data] No vertices.\n");
        return false;
    }


    if(m_hasColors && (!colors.empty())){
        if(colors.size() == m_nbVertices){
            colorsB.load_data(
                reinterpret_cast<const GLfloat*>(colors.data()),
                static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat)),
                colorBufferUsage
            );
        }else{
            Logger::error("[VoxelsRenderer::load_data] Invalid color buffer size.\n");
            return false;
        }
    }

    m_dataLoaded = true;

    return true;
}

auto tool::gl::VoxelsRenderer::update_data(std::span<const geo::Pt3<int> > vertices, size_t verticesOffset, std::span<const geo::Pt3f> colors, size_t colorsOffset) -> bool{


    if(!initialized()){
        Logger::error("[VoxelsRenderer::update_data] Buffers must be initialized.\n");
        return false;
    }

    if(!data_loaded()){
        Logger::error("[VoxelsRenderer::update_data] Data must be loaded.\n");
        return false;
    }

    if(m_nbVertices == 0){
        Logger::error("[VoxelsRenderer::update_data] No vertices.\n");
        return false;
    }

    if(!vertices.empty()){

        if(!(positionBufferUsage & GL_DYNAMIC_STORAGE_BIT)){
            Logger::error("[VoxelsRenderer::update_data] Vertex buffer storage not dynamic.\n");
            return false;
        }

        if((verticesOffset + vertices.size()*3) <= m_nbVertices*3){
            pointsB.update_data(
                reinterpret_cast<const GLint*>(vertices.data()),
                static_cast<GLsizeiptr>(vertices.size()*3*sizeof(GLint)),
                static_cast<GLintptr>(verticesOffset)
            );
        }else{
            Logger::error("[VoxelsRenderer::update_data] Invalid vertex buffer size.\n");
            return false;
        }
    }

    if(m_hasColors && (!colors.empty())){

        if(!(colorBufferUsage & GL_DYNAMIC_STORAGE_BIT)){
            Logger::error("[VoxelsRenderer::update_data] Color buffer storage not dynamic.\n");
            return false;
        }

        if((colorsOffset + colors.size()*3) <= m_nbVertices*3){
            colorsB.update_data(
                reinterpret_cast<const GLfloat*>(colors.data()),
                static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat)),
                static_cast<GLintptr>(colorsOffset)
            );
        }else{
            Logger::error("[VoxelsRenderer::update_data] Invalid color buffer size.\n");
            return false;
        }
    }

    return true;
}

auto tool::gl::VoxelsRenderer::generate_vbo() -> void{
    pointsB.initialize();
    if(m_hasColors){
        colorsB.initialize();
    }
}

auto tool::gl::VoxelsRenderer::vertex_array_vertex_buffer() -> void{

    GL::vertex_array_vertex_buffer(
        vao.id(),
        positionBindingId,
        pointsB.id(),
        0,
        sizeof(GLint)*3
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

auto tool::gl::VoxelsRenderer::vertex_array_attrib_format() -> void{

    GL::vertex_array_attrib_format(
        vao.id(),
        positionBindingId,
        3,
        GL_INT,
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

auto tool::gl::VoxelsRenderer::enable_vertex_array_attrib() -> void{

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

auto tool::gl::VoxelsRenderer::vertex_array_attrib_binding() -> void{

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
