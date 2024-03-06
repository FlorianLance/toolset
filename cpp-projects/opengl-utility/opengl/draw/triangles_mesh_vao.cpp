
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

auto TriangleMeshVAO::init_and_load_3d_mesh(
    std::span<const geo::Pt3<GLuint>> indices,
    std::span<const Pt3f> points,
    std::span<const Pt3f> normals,
    std::span<const Pt2f> texCoords,
    std::span<const Pt4f> tangents,
    std::span<const graphics::BoneData> bones,
    std::span<const ColorRGBA32> colors) -> void{

    if(points.empty() || indices.empty()){
        Logger::error("[TriangleMesh::init_buffers] No points.\n");
        return;
    }

    bool hasNormals  = normals.size()   == points.size();
    bool hasTexCoord = texCoords.size() == points.size();
    bool hasTangents = tangents.size()  == points.size();
    bool hasBones    = bones.size()     == points.size();
    bool hasColors   = colors.size()    == points.size();

    // generate vao
    vao.generate();

    // generate vbo
    pointsB.generate();
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

    if(hasNormals){

        normalsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(normals.size()*3*sizeof(GLfloat))
        );

        normalsB.dsa_attrib(
            vao.id(),
            1,
            3,
            sizeof(Pt3f),
            GL_FLOAT,
            0,
            1
        );
    }

    if(hasTexCoord){

        texCoordsB.load_data(
            reinterpret_cast<const GLfloat*>(normals.data()),
            static_cast<GLsizeiptr>(texCoords.size()*2*sizeof(GLfloat))
        );

        texCoordsB.dsa_attrib(
            vao.id(),
            2,
            2,
            sizeof(Pt2f),
            GL_FLOAT,
            0,
            2
        );
    }

    if(hasTangents){

        tangentsB.load_data(
            reinterpret_cast<const GLfloat*>(tangents.data()),
            static_cast<GLsizeiptr>(tangents.size()*3*sizeof(GLfloat))
        );

        tangentsB.dsa_attrib(
            vao.id(),
            3,
            3,
            sizeof(Pt3f),
            GL_FLOAT,
            0,
            3
         );
    }

    if(hasBones){

        bonesB.load_data(
            reinterpret_cast<const GLvoid*>(bones.data()),
            static_cast<GLsizeiptr>(bones.size()*sizeof(BoneData))
        );




        // Bind a buffer to a vertex buffer bind point.
        // glVertexArrayVertexBuffer(
        //     vaobj,          // Specifies the name of the vertex array object to be used by glVertexArrayVertexBuffer function.
        //     bindingindex,   // The index of the vertex buffer binding point to which to bind the buffer.
        //     buffer,         // The name of a buffer to bind to the vertex buffer binding point.
        //     offset,         // The offset of the first element of the buffer.
        //     stride          // The distance between elements within the buffer.
        // );

        // // Enable or disable a generic vertex attribute array.
        // glEnableVertexArrayAttrib(
        //     vaobj,  // Specifies the name of the vertex array object for glDisableVertexArrayAttrib and glEnableVertexArrayAttrib functions.
        //     index   // Specifies the index of the generic vertex attribute to be enabled or disabled.
        // );

        // // Specify the organization of vertex arrays
        // glVertexArrayAttribFormat(
        //     vao.id(),       // Specifies the name of the vertex array object for glVertexArrayAttrib{I, L}Format functions.
        //     attribindex,    // The generic vertex attribute array being described.
        //     size,           // The number of values per vertex that are stored in the array.
        //     type,           // he type of the data stored in the array.
        //     normalized,     // Specifies whether fixed-point data values should be normalized (GL_TRUE)
        //     // or converted directly as fixed-point values (GL_FALSE) when they are accessed. This parameter is ignored if type is GL_FIXED.
        //     relativeoffset  // The distance between elements within the buffer.
        // );

        // // Associate a vertex attribute and a vertex buffer binding for a vertex array object
        // glVertexArrayAttribBinding(
        //     vaobj,          // Specifies the name of the vertex array object for glVertexArrayAttribBinding.
        //     attribindex,    // The index of the attribute to associate with a vertex buffer binding.
        //     bindingindex    // The index of the vertex buffer binding with which to associate the generic vertex attribute.
        // );


        // if(type.v == GL_INT){
        //     GL::vertex_attrib_i_pointer(index.v, size.v, type.v, stride.v,  offset.v);
        // }else{
        //     GL::vertex_attrib_pointer(index.v, size.v, type.v, GL_FALSE, stride.v,  offset.v);
        // }

        // define an array of generic vertex attribute data
        // void glVertexAttribIPointer(
        //     GLuint index,
        //     GLint size,
        //     GLenum type,
        //     GLsizei stride,
        //     const void * pointer
        // );



        // glBindBuffer(GL_ARRAY_BUFFER, bonesB.id());
        // glBufferData(GL_ARRAY_BUFFER, sizeof(graphics::BoneData) * bones->size(), &bones->at(0), GL_STATIC_DRAW);

        // glEnableVertexAttribArray(4);
        // glVertexAttribIPointer(4, 4, GL_INT, sizeof(graphics::BoneData), (const GLvoid*)0);

        // glEnableVertexAttribArray(5);
        // glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(graphics::BoneData), (const GLvoid*)(4* sizeof(float)));


    }
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

    nIndices = static_cast<GLsizei>(indices->size());

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

    nIndices = static_cast<GLsizei>(indices->size()*3);

    VAO::unbind();
    buffersInitialized = true;
}

auto TriangleMeshVAO::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_triangles_with_ebo(nIndices);
    VAO::unbind();
}

auto TriangleMeshVAO::render_adjacency() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_triangles_adjacency_with_ebo(nIndices);
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






