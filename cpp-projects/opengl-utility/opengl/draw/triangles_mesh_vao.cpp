
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

#include "triangles_mesh_vao.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "opengl/gl_draw.hpp"

using namespace tool::gl;
using namespace tool::geo;
using namespace tool::graphics;

auto TriangleMeshVAO::generate_bo() -> void{

    indicesB.generate();
    pointsB.generate();
    if(m_hasNormals){
        normalsB.generate();
    }
    if(m_hasTexCoord){
        texCoordsB.generate();
    }
    if(m_hasTangents){
        tangentsB.generate();
    }
    if(m_hasBones){
        // bonesB.generate();
    }
    if(m_hasColors){
        colorsB.generate();
    }    
}

auto TriangleMeshVAO::load_data(
    std::span<const geo::Pt3<GLuint>> indices,
    std::span<const geo::Pt3f> points,
    std::span<const geo::Pt3f> normals,
    std::span<const geo::Pt2f> texCoords,
    std::span<const geo::Pt4f> tangents,
    std::span<const graphics::BoneData> bones,
    std::span<const ColorRGBA32> colors) -> void{

    if(!indices.empty()){
        indicesB.load_data(
            reinterpret_cast<const GLuint *>(indices.data()),
            GLsizeiptr(indices.size()*3*sizeof(std::uint32_t)),
            indicesBufferUsage
        );
    }

    if(!points.empty()){
        pointsB.load_data(
            reinterpret_cast<const GLfloat*>(points.data()),
            static_cast<GLsizeiptr>(points.size()*3*sizeof(GLfloat)),
            positionBufferUsage
        );
    }

    if(m_hasNormals && !normals.empty()){
        normalsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat)),
            normalBufferUsage
        );
    }

    if(m_hasTexCoord && !texCoords.empty()){
        texCoordsB.load_data(
            reinterpret_cast<const GLfloat*>(texCoords.data()),
            static_cast<GLsizeiptr>(texCoords.size()*2*sizeof(GLfloat)),
            texCoordsBufferUsage
        );
    }

    if(m_hasTangents && !tangents.empty()){
        tangentsB.load_data(
            reinterpret_cast<const GLfloat*>(tangents.data()),
            static_cast<GLsizeiptr>(tangents.size()*4*sizeof(GLfloat)),
            tangentsBufferUsage
        );
    }

    if(m_hasBones && !bones.empty()){
        // bonesB.load_data(
        //     reinterpret_cast<const GLvoid*>(bones.data()),
        //     static_cast<GLsizeiptr>(bones.size()*sizeof(BoneData)),
        //     bonesBufferUsage
        // );
    }

    if(m_hasColors && !colors.empty()){
        colorsB.load_data(
            reinterpret_cast<const GLfloat*>(colors.data()),
            static_cast<GLsizeiptr>(colors.size()*4*sizeof(GLfloat)),
            colorBufferUsage
        );
    }
}

auto TriangleMeshVAO::vertex_array_vertex_buffer() -> void{

    GL::vertex_array_vertex_buffer(
        vao.id(),
        positionBindingId,
        pointsB.id(),
        0,
        sizeof(GLfloat)*3
    );

    if(m_hasNormals){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            normalBindingId,
            normalsB.id(),
            0,
            sizeof(GLfloat)*3
        );
    }

    if(m_hasTexCoord){

        GL::vertex_array_vertex_buffer(
            vao.id(),
            texCoordsBindingId,
            texCoordsB.id(),
            0,
            sizeof(GLfloat)*2
        );
    }

    if(m_hasTangents){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            tangentsBindingId,
            tangentsB.id(),
            0,
            sizeof(GLfloat)*4
        );
    }

    if(m_hasBones){
        // ...
    }

    if(m_hasColors){
        GL::vertex_array_vertex_buffer(
            vao.id(),
            colorBindingId,
            colorsB.id(),
            0,
            sizeof(GLfloat)*4
        );
    }
}

auto TriangleMeshVAO::vertex_array_attrib_format() -> void{

    GL::vertex_array_attrib_format(
        vao.id(),
        positionBindingId,
        3,
        GL_FLOAT,
        GL_FALSE,
        0
    );

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

    if(m_hasTexCoord){
        GL::vertex_array_attrib_format(
            vao.id(),
            texCoordsBindingId,
            2,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }

    if(m_hasTangents){
        GL::vertex_array_attrib_format(
            vao.id(),
            tangentsBindingId,
            4,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }

    if(m_hasBones){
        // ...
    }

    if(m_hasColors){
        GL::vertex_array_attrib_format(
            vao.id(),
            colorBindingId,
            4,
            GL_FLOAT,
            GL_FALSE,
            0
        );
    }
}

auto TriangleMeshVAO::enable_vertex_array_attrib() -> void{

    GL::enable_vertex_array_attrib(
        vao.id(),
        positionBindingId
    );

    if(m_hasNormals){
        GL::enable_vertex_array_attrib(
            vao.id(),
            normalBindingId
        );
    }

    if(m_hasTexCoord){
        GL::enable_vertex_array_attrib(
            vao.id(),
            texCoordsBindingId
        );
    }

    if(m_hasTangents){
        GL::enable_vertex_array_attrib(
            vao.id(),
            tangentsBindingId
        );
    }

    if(m_hasBones){
        // ...
    }

    if(m_hasColors){
        GL::enable_vertex_array_attrib(
            vao.id(),
            colorBindingId
        );
    }
}

auto TriangleMeshVAO::vertex_array_attrib_binding() -> void{

    GL::vertex_array_attrib_binding(
        vao.id(),
        positionLoc,
        positionBindingId
    );

    if(m_hasNormals){
        GL::vertex_array_attrib_binding(
            vao.id(),
            normalLoc,
            normalBindingId
        );
    }

    if(m_hasTexCoord){
        GL::vertex_array_attrib_binding(
            vao.id(),
            texCoordsLoc,
            texCoordsBindingId
        );
    }

    if(m_hasTangents){
        GL::vertex_array_attrib_binding(
            vao.id(),
            tangentsLoc,
            tangentsBindingId
        );
    }

    if(m_hasBones){
        // ...
    }

    if(m_hasColors){
        GL::vertex_array_attrib_binding(
            vao.id(),
            colorLoc,
            colorBindingId
        );
    }
}


auto TriangleMeshVAO::init_and_load_3d_mesh(
    std::span<const geo::Pt3<GLuint>> indices,
    std::span<const Pt3f> points,
    std::span<const Pt3f> normals,
    std::span<const Pt2f> texCoords,
    std::span<const Pt4f> tangents,
    std::span<const graphics::BoneData> bones,
    std::span<const ColorRGBA32> colors) -> void{

    if(indices.empty()){
        Logger::error("[TriangleMeshVAO::init_and_load_3d_mesh] No indices.\n");
        return;
    }

    if(points.empty()){
        Logger::error("[TriangleMeshVAO::init_and_load_3d_mesh] No points.\n");
        return;
    }

    if(buffersInitialized){
        clean();
    }

    m_hasNormals  = normals.size()   == points.size();
    m_hasTexCoord = texCoords.size() == points.size();
    m_hasTangents = tangents.size()  == points.size();
    m_hasBones    = bones.size()     == points.size();
    m_hasColors   = colors.size()    == points.size();

    vao.generate();    
    generate_bo();

    vao.bind();
    indicesB.bind();

    load_data(indices, points, normals, texCoords, tangents, bones, colors);

    vertex_array_vertex_buffer();
    vertex_array_attrib_format();
    enable_vertex_array_attrib();
    vertex_array_attrib_binding();

    nIndicesAllocated = static_cast<GLsizei>(indices.size()*3);
    buffersInitialized = true;

    VAO::unbind();
    indicesB.unbind();

}

auto TriangleMeshVAO::init_buffers(std::vector<GLuint> *indices, std::vector<GLfloat> *points, std::vector<GLfloat> *normals, std::vector<GLfloat> *texCoords, std::vector<GLfloat> *tangents, std::vector<GLfloat> *colors) -> void{

    if(indices == nullptr || points == nullptr){
        Logger::error("[TriangleMesh::init_buffers] error, no indices or points buffers.\n");
        return;
    }

    if(normals != nullptr){
        hasNormals = points->size() == normals->size();

        if(!hasNormals && normals->size() > 0){
            Logger::error("[TriangleMesh::init_buffers] Invalid size of normals.\n");
            return;
        }
    }

    if(texCoords != nullptr){
        hasTexCoord = (points->size()/3) == (texCoords->size()/2);

        if(!hasTexCoord && texCoords->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of tex coord.\n");
            return;
        }
    }

    if(tangents != nullptr){
        hasTangents = (points->size()/3) == (tangents->size()/4);

        if(!hasTangents && tangents->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of tangents.\n");
            return;
        }
    }

    if(colors != nullptr){
        hasColors = (points->size()/3) == (colors->size()/4);

        if(!hasColors && colors->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of colors.\n");
            return;
        }
    }

    // clean if necessary
    if(buffersInitialized){
        clean();
    }

    vao.generate();
    indicesB.generate();

    // generate buffers
    pointsB.generate();
    pointsB.load_data(
        points->data(),
        static_cast<GLsizeiptr>(points->size()*sizeof(GLfloat))
    );
    //    pointsB.dsa_attrib(vao.id(), AttriIndex{0}, AttriSize{3}, AttriType{GL_FLOAT}, Stride{0}, RelativeOffset{0});


    if(hasNormals){
        normalsB.generate();
        normalsB.load_data(
            normals->data(),
            static_cast<GLsizeiptr>(normals->size()*sizeof(GLfloat))
        );
        //        normalsB.dsa_attrib(vao.id(), AttriIndex{1}, AttriSize{3}, AttriType{GL_FLOAT}, Stride{0}, RelativeOffset{0});
    }
    if(hasTexCoord){
        texCoordsB.generate();
        texCoordsB.load_data(
            texCoords->data(),
            static_cast<GLsizeiptr>(texCoords->size()*sizeof(GLfloat))
            );
        //        texCoordsB.dsa_attrib(vao.id(), AttriIndex{2}, AttriSize{2}, AttriType{GL_FLOAT}, Stride{0}, RelativeOffset{0});
    }
    if(hasTangents){
        tangentsB.generate();
        tangentsB.load_data(
            tangents->data(),
            static_cast<GLsizeiptr>(tangents->size()*sizeof(GLfloat))
        );
        //        tangentsB.dsa_attrib(vao.id(), AttriIndex{3}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{0}, RelativeOffset{0});
    }

    if(hasColors){
        colorsB.generate();
        colorsB.load_data(
            colors->data(),
            static_cast<GLsizeiptr>(colors->size()*sizeof(GLfloat))
        );
    }

    vao.bind();

    // indices
    indicesB.bind();
    indicesB.load_data(indices->data(), GLsizeiptr(indices->size()*sizeof(std::uint32_t)));

    // points
    pointsB.attrib(AttriIndex{0}, AttriSize{3}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});

    // normals
    if(hasNormals){
        normalsB.attrib(AttriIndex{1}, AttriSize{3}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }

    // tex coords
    if(hasTexCoord){
        texCoordsB.attrib(AttriIndex{2}, AttriSize{2}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }

    // tangents
    if(hasTangents){
        tangentsB.attrib(AttriIndex{3}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }

    // colors
    if(hasColors){
        colorsB.attrib(AttriIndex{4}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }
    
    nIndicesAllocated = static_cast<GLsizei>(indices->size());

    VAO::unbind();
    buffersInitialized = true;
}

auto TriangleMeshVAO::init_buffers(
    std::vector<TriIds> *indices, std::vector<Pt3f> *points, std::vector<Pt3f> *normals,
    std::vector<Pt2f> *texCoords, std::vector<Pt4f> *tangents, std::vector<graphics::BoneData> *bones, std::vector<ColorRGBA32> *colors) -> void{

    if(indices == nullptr || points == nullptr){
        Logger::error("[TriangleMeshData::init_buffers] error, no indices or points buffers.\n");
        return;
    }

    if(normals != nullptr){
        hasNormals = points->size() == normals->size();

        if(!hasNormals && normals->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of normals.\n");
            return;
        }
    }

    if(texCoords != nullptr){
        hasTexCoord = points->size() == texCoords->size();

        if(!hasTexCoord && texCoords->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of tex coord.\n");
            return;
        }
    }

    if(tangents != nullptr){
        hasTangents = points->size() == tangents->size();

        if(!hasTangents && tangents->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of tangents.\n");
            return;
        }
    }

    if(bones != nullptr){
        hasBones = points->size() == bones->size();

        if(!hasBones && bones->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of bones.\n");
            return;
        }
    }

    if(colors != nullptr){
        hasColors = points->size() == colors->size();

        if(!hasColors && colors->size() > 0){
            Logger::error("[TriangleMeshData::init_buffers] Invalid size of colors.\n");
            return;
        }
    }


    // clean if necessary
    if(buffersInitialized){
        clean();
    }

    vao.generate();

    // generate buffers
    pointsB.generate();
    indicesB.generate();
    if(hasNormals){
        normalsB.generate();
    }
    if(hasTexCoord){
        texCoordsB.generate();
    }
    if(hasTangents){
        tangentsB.generate();
    }
    if(hasBones){
        bonesB.generate();
    }
    if(hasColors){
        colorsB.generate();
    }

    vao.bind();


    // convert indices
    std::vector<GLuint> rawIndices;
    rawIndices.reserve(indices->size()*3);
    for(auto &i : *indices){
        rawIndices.emplace_back(static_cast<GLuint>(i.id1()));
        rawIndices.emplace_back(static_cast<GLuint>(i.id2()));
        rawIndices.emplace_back(static_cast<GLuint>(i.id3()));
    }

    // indices
    indicesB.bind();
    indicesB.load_data(rawIndices.data(), GLsizeiptr(indices->size()*3*sizeof(std::uint32_t)));

    // points
    pointsB.load_data(points->data()->array.data(), static_cast<GLsizeiptr>(points->size()*3*sizeof(GLfloat)));
    pointsB.attrib(AttriIndex{0}, AttriSize{3}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});

    if(hasNormals){
        normalsB.load_data(
            normals->data()->array.data(),
            static_cast<GLsizeiptr>(normals->size()*3*sizeof(GLfloat))
            );
        normalsB.attrib(AttriIndex{1}, AttriSize{3}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }

    // tex coords
    if(hasTexCoord){
        texCoordsB.load_data(
            texCoords->data()->array.data(),
            static_cast<GLsizeiptr>(texCoords->size()*2*sizeof(GLfloat))
            );
        texCoordsB.attrib(AttriIndex{2}, AttriSize{2}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }

    // tangents
    if(hasTangents){
        tangentsB.load_data(
            tangents->data()->array.data(),
            static_cast<GLsizeiptr>(tangents->size()*4*sizeof(GLfloat))
            );
        tangentsB.attrib(AttriIndex{3}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }

    // bones
    if(hasBones){

        //        glNamedBufferStorage(
        //            bonesB.id(),   // GLuint buffer
        //            static_cast<GLsizeiptr>(sizeof (BoneData)* bones->size()), // GLsizeiptr size
        //            bones->data(), // const void *data
        //            GL_DYNAMIC_STORAGE_BIT // GLenum usage
        //        );

        //        glBindBuffer(GL_ARRAY_BUFFER, bonesB.id());
        //        glEnableVertexAttribArray(4);
        //        glVertexAttribIPointer(4, 4, GL_INT, sizeof (BoneData), reinterpret_cast<GLvoid*>(0* sizeof(float)));
        //        glEnableVertexAttribArray(5);
        //        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof (BoneData), reinterpret_cast<GLvoid*>(16* sizeof(float)));

        //        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //        bonesB.attrib(AttriIndex{4}, AttriSize{4}, AttriType{GL_INT}, Stride{sizeof (BoneData)}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
        //        bonesB.attrib(AttriIndex{5}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{sizeof (BoneData)}, AttribOffset{reinterpret_cast<GLvoid*>(16* sizeof(float))});


        glBindBuffer(GL_ARRAY_BUFFER, bonesB.id());
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphics::BoneData) * bones->size(), &bones->at(0), GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(4, 4, GL_INT, sizeof(graphics::BoneData), (const GLvoid*)0);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(graphics::BoneData), (const GLvoid*)(4* sizeof(float)));
    }

    if(hasColors){
        colorsB.load_data(
            colors->data()->array.data(),
            static_cast<GLsizeiptr>(colors->size()*4*sizeof(GLfloat))
            );
        colorsB.attrib(AttriIndex{6}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{0}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
    }
    
    nIndicesAllocated = static_cast<GLsizei>(indices->size()*3);

    VAO::unbind();
    buffersInitialized = true;
}

auto TriangleMeshVAO::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_triangles_with_ebo(nIndicesAllocated);
    VAO::unbind();
}

auto TriangleMeshVAO::render_adjacency() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_triangles_adjacency_with_ebo(nIndicesAllocated);
    VAO::unbind();
}


auto TriangleMeshVAO::clean() -> void{
    vao.clean();
    pointsB.clean();
    indicesB.clean();
    normalsB.clean();
    tangentsB.clean();
    texCoordsB.clean();
    buffersInitialized = false;
}






