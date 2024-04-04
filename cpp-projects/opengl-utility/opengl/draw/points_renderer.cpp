
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

#include "points_renderer.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::geo;
using namespace tool::gl;


auto PointsRenderer::initialize(bool hasColors, bool hasNormals) -> void{

    if(m_buffersInitialized){
        clean();
    }

    m_hasColors  = hasColors;
    m_hasNormals = hasNormals;

    vao.initialize();
    generate_vbo();

    vertex_array_vertex_buffer();
    vertex_array_attrib_format();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();

    m_buffersInitialized = true;
}


auto PointsRenderer::clean() -> void{

    vao.clean();

    pointsB.clean();
    colorsB.clean();
    normalsB.clean();

    m_buffersInitialized = false;

    m_nbIndices = 0;
    m_nbVertices = 0;

    m_hasColors = false;
    m_hasNormals = false;

    positionBufferUsage = 0;
    colorBufferUsage = 0;
    normalBufferUsage = 0;
}


auto PointsRenderer::load_data(
    std::span<const geo::Pt3f> vertices,
    std::span<const geo::Pt3f> colors,
    std::span<const geo::Pt3f> normals
    ) -> bool{

    if(!vertices.empty()){
        pointsB.load_data(
            reinterpret_cast<const GLfloat*>(vertices.data()),
            static_cast<GLsizeiptr>(vertices.size()*3*sizeof(GLfloat)),
            positionBufferUsage
        );
        m_nbVertices = static_cast<GLsizei>(vertices.size());
        m_nbIndices  = m_nbVertices;
    }

    if(m_nbVertices == 0){
        Logger::error("[PointsRenderer::load_data] No vertices.\n");
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
            Logger::error("[PointsRenderer::load_data] Invalid color buffer size.\n");
            return false;
        }
    }

    if(m_hasNormals && (!normals.empty())){
        if(normals.size() == m_nbVertices){
            normalsB.load_data(
                reinterpret_cast<const GLfloat*>(normals.data()),
                static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat)),
                normalBufferUsage
            );
        }else{
            Logger::error("[PointsRenderer::load_data] Invalid normal buffer size.\n");
            return false;
        }
    }

    m_dataLoaded = true;

    return true;
}

auto PointsRenderer::update_data(
    std::span<const Pt3f> vertices, size_t verticesOffset ,
    std::span<const Pt3f> colors,   size_t colorsOffset,
    std::span<const Pt3f> normals,  size_t normalsOffset
    ) -> bool{

    if(!initialized()){
        Logger::error("[PointsRenderer::update_data] Buffers must be initialized.\n");
        return false;
    }

    if(!data_loaded()){
        Logger::error("[PointsRenderer::update_data] Data must be loaded.\n");
        return false;
    }

    if(m_nbVertices == 0){
        Logger::error("[PointsRenderer::update_data] No vertices.\n");
        return false;
    }

    if(!vertices.empty()){

        if(!(positionBufferUsage & GL_DYNAMIC_STORAGE_BIT)){
            Logger::error("[PointsRenderer::update_data] Vertex buffer storage not dynamic.\n");
            return false;
        }

        if((verticesOffset + vertices.size()*3) <= m_nbVertices*3){
            pointsB.update_data(
                reinterpret_cast<const GLfloat*>(vertices.data()),
                static_cast<GLsizeiptr>(vertices.size()*3*sizeof(GLfloat)),
                static_cast<GLintptr>(verticesOffset)
            );
        }else{
            Logger::error("[PointsRenderer::update_data] Invalid vertex buffer size.\n");
            return false;
        }
    }

    if(m_hasColors && (!colors.empty())){

        if(!(colorBufferUsage & GL_DYNAMIC_STORAGE_BIT)){
            Logger::error("[PointsRenderer::update_data] Color buffer storage not dynamic.\n");
            return false;
        }

        if((colorsOffset + colors.size()*3) <= m_nbVertices*3){
            colorsB.update_data(
                reinterpret_cast<const GLfloat*>(colors.data()),
                static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat)),
                static_cast<GLintptr>(colorsOffset)
            );
        }else{
            Logger::error("[PointsRenderer::update_data] Invalid color buffer size.\n");
            return false;
        }
    }

    if(m_hasNormals && (!normals.empty())){

        if(!(normalBufferUsage & GL_DYNAMIC_STORAGE_BIT)){
            Logger::error("[PointsRenderer::update_data] Color buffer storage not dynamic.\n");
            return false;
        }

        if((normalsOffset + normals.size()*3) <= m_nbVertices*3){
            normalsB.update_data(
                reinterpret_cast<const GLfloat*>(normals.data()),
                static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat)),
                static_cast<GLintptr>(normalsOffset)
            );
        }else{
            Logger::error("[PointsRenderer::update_data] Invalid color buffer size.\n");
            return false;
        }
    }

    return true;
}


auto PointsRenderer::generate_vbo() -> void{

    pointsB.initialize();
    if(m_hasColors){
        colorsB.initialize();
    }
    if(m_hasNormals){
        normalsB.initialize();
    }
}

auto PointsRenderer::vertex_array_vertex_buffer_for_2d_points() -> void{

    GL::vertex_array_vertex_buffer(
        vao.id(),
        positionBindingId,
        pointsB.id(),
        0,
        sizeof(GLfloat)*2
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
    if(m_hasNormals){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            normalBindingId,
            normalsB.id(),
            0,
            sizeof(GLfloat)*2
        );
    }
}

auto PointsRenderer::vertex_array_vertex_buffer() -> void{

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
    if(m_hasNormals){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            normalBindingId,
            normalsB.id(),
            0,
            sizeof(GLfloat)*3
        );
    }
}

auto PointsRenderer::vertex_array_vertex_buffer_for_3d_voxels() -> void{

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

auto PointsRenderer::vertex_array_attrib_format_for_2d_points() -> void{

    GL::vertex_array_attrib_format(
        vao.id(),
        positionBindingId,
        2,
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
    if(m_hasNormals){
        GL::vertex_array_attrib_format(
            vao.id(),
            normalBindingId,
            2,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }
}


auto PointsRenderer::vertex_array_attrib_format() -> void{

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
    if(m_hasNormals){
        GL::vertex_array_attrib_format(
            vao.id(),
            normalBindingId,
            3,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }
}

auto PointsRenderer::vertex_array_attrib_format_for_3d_voxels() -> void{

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

auto PointsRenderer::enable_vertex_array_attrib() -> void{

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
    if(m_hasNormals){
        GL::enable_vertex_array_attrib(
            vao.id(),
            normalBindingId
        );
    }
}



auto PointsRenderer::vertex_array_attrib_binding() -> void{

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
    if(m_hasNormals){
        GL::vertex_array_attrib_binding(
            vao.id(),
            normalLoc,
            normalBindingId
        );
    }
}


auto PointsRenderer::initialize(size_t size, bool hasColors, bool hasNormals) -> void{

    if(size == 0){
        Logger::error("[PointsMesh::init_3d_points] No points.\n");
        return;
    }
    
    if(m_buffersInitialized){
        clean();
    }

    m_hasColors  = hasColors;
    m_hasNormals = hasNormals;

    vao.initialize();
    generate_vbo();

    // load vbo
    const GLfloat *pointsData = nullptr;
    pointsB.load_data(
        pointsData,
        static_cast<GLsizeiptr>(size*3*sizeof(GLfloat)),
        positionBufferUsage
    );
    if(m_hasColors){
        const GLfloat *colorsData = nullptr;
        colorsB.load_data(
            colorsData,
            static_cast<GLsizeiptr>(size*3*sizeof(GLfloat)),
            colorBufferUsage
        );
    }
    if(m_hasNormals){
        const GLfloat *normalsData = nullptr;
        normalsB.load_data(
            normalsData,
            static_cast<GLsizeiptr>(size*3*sizeof(GLfloat)),
            normalBufferUsage
        );
    }

    vertex_array_vertex_buffer();
    vertex_array_attrib_format();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();
    
    m_nbIndices = static_cast<GLsizei>(size);
    m_buffersInitialized = true;
}

auto PointsRenderer::init_and_load_2d_points(std::span<const Pt2f> points, std::span<const Pt3f> colors, std::span<const Pt2f> normals) -> void {

    if(points.empty()){
        Logger::error("[PointsMesh::init_and_load_2d_points] No points.\n");
        return;
    }
    
    if(m_buffersInitialized){
        clean();
    }

    m_hasColors  = colors.size() == points.size();
    m_hasNormals = normals.size() == points.size();

    vao.initialize();
    generate_vbo();

    // load vbo
    pointsB.load_data(
        reinterpret_cast<const GLfloat*>(points.data()),
        static_cast<GLsizeiptr>(points.size()*2*sizeof(GLfloat)),
        positionBufferUsage
    );
    if(m_hasColors){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat)),
            colorBufferUsage
        );
    }
    if(m_hasNormals){
        normalsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*2*sizeof(GLfloat)),
            normalBufferUsage
        );
    }

    vertex_array_vertex_buffer_for_2d_points();
    vertex_array_attrib_format_for_2d_points();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();
    
    m_nbIndices  = static_cast<GLsizei>(points.size());
    m_buffersInitialized = true;
    VAO::unbind();
}

auto PointsRenderer::update_3d_points(std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals, GLintptr offset) -> void{
    
    if(!m_buffersInitialized){
        Logger::error("[PointsMesh::load_3d_points] Buffer not initialized.\n");
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

    if(m_hasNormals){
        normalsB.update_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat)),
            offset
        );
    }
}

auto PointsRenderer::init_and_load_3d_points(std::span<const Pt3f> points, std::span<const Pt3f> colors, std::span<const Pt3f> normals) -> void{

    if(points.empty()){
        Logger::error("[PointsMesh::init_and_load_3d_points] No points.\n");
        return;
    }
    
    if(m_buffersInitialized){
        clean();
    }

    m_hasColors  = colors.size() == points.size();
    m_hasNormals = normals.size() == points.size();

    vao.initialize();
    generate_vbo();

    // load vbo
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
    if(m_hasNormals){
        normalsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat)),
            normalBufferUsage
        );
    }

    vertex_array_vertex_buffer();
    vertex_array_attrib_format();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();
    m_nbIndices = static_cast<GLsizei>(points.size());
    m_buffersInitialized = true;
}

auto PointsRenderer::init_and_load_3d_voxels(std::span<const Pt3<int>> voxels, std::span<const Pt3f> colors) -> void{

    if(voxels.empty()){
        Logger::error("[PointsMesh::init_and_load_3d_voxels] No voxels.\n");
        return;
    }
    
    if(m_buffersInitialized){
        clean();
    }

    m_hasColors  = colors.size() == voxels.size();
    m_hasNormals = false;


    vao.initialize();
    generate_vbo();

    // load vbo
    pointsB.load_data(
        reinterpret_cast<const GLint*>(voxels.data()),
        static_cast<GLsizeiptr>(voxels.size()*3*sizeof(GLint))
    );
    if(m_hasColors){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*3*sizeof(GLfloat))
        );
    }

    vertex_array_vertex_buffer_for_3d_voxels();
    vertex_array_attrib_format_for_3d_voxels();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();
    m_nbIndices = static_cast<GLsizei>(voxels.size());
    m_buffersInitialized = true;
}

auto PointsRenderer::render() const -> void{
    
    if(!m_buffersInitialized){
        return;
    }

    vao.bind();
    GL::draw_arrays_instance_base_instance(GL_POINTS, 0, m_nbIndices, 1, 0);
    VAO::unbind();
}

auto PointsRenderer::render_patches() const -> void{
    
    if(!m_buffersInitialized){
        return;
    }

    vao.bind();
    GL::draw_arrays_instance_base_instance(GL_PATCHES, 0, m_nbIndices, 1, 0);
    VAO::unbind();
}





