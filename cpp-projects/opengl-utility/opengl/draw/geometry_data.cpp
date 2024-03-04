
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

#include "geometry_data.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_draw.hpp"

using namespace tool;
using namespace tool::gl;


auto LineMeshData::init_buffers(std::vector<GLuint> *indices, std::vector<GLfloat> *points, std::vector<GLfloat> *colors) -> void{

    // check inputs
    if(indices == nullptr || points == nullptr){
        Logger::error("[LineMeshData::init_buffers] error, no indices or points buffers.\n");
        return;
    }

    if(indices->size() == 0 || points->size() == 0){
        Logger::error("[LineMeshData::init_buffers] error, empty buffers.\n");
        return;
    }

    if(colors != nullptr){
        if(points->size()/3 != colors->size()/4){
            Logger::error("[LineMeshData::init_buffers] error, different size for points and colors buffers.\n");
            return;
        }
    }

    // clean buffers
    if(buffersInitialized){
        clean();
    }

    // generate buffers
    vao.generate();
    pointsB.generate();
    indicesB.generate();
    if(colors != nullptr){
        colorsB.generate();
    }

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

auto LineMeshData::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_lines_with_ebo(nIndices);
    VAO::unbind();
}

auto LineMeshData::clean() -> void{
    vao.clean();
    pointsB.clean();
    indicesB.clean();
    colorsB.clean();
    buffersInitialized = false;
}

auto TriangleMeshData::init_buffers(std::vector<GLuint> *indices, std::vector<GLfloat> *points, std::vector<GLfloat> *normals, std::vector<GLfloat> *texCoords, std::vector<GLfloat> *tangents, std::vector<GLfloat> *colors) -> void{

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

auto TriangleMeshData::init_buffers(
    std::vector<geo::TriIds> *indices, std::vector<geo::Pt3f> *points, std::vector<geo::Pt3f> *normals,
    std::vector<geo::Pt2f> *texCoords, std::vector<geo::Pt4f> *tangents, std::vector<graphics::BoneData> *bones, std::vector<ColorRGBA32> *colors) -> void{

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
        rawIndices.emplace_back(static_cast<GLuint>(i.ids.x()));
        rawIndices.emplace_back(static_cast<GLuint>(i.ids.y()));
        rawIndices.emplace_back(static_cast<GLuint>(i.ids.z()));
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
        //            static_cast<GLsizeiptr>(sizeof (geo::BoneData)* bones->size()), // GLsizeiptr size
        //            bones->data(), // const void *data
        //            GL_DYNAMIC_STORAGE_BIT // GLenum usage
        //        );

        //        glBindBuffer(GL_ARRAY_BUFFER, bonesB.id());
        //        glEnableVertexAttribArray(4);
        //        glVertexAttribIPointer(4, 4, GL_INT, sizeof (geo::BoneData), reinterpret_cast<GLvoid*>(0* sizeof(float)));
        //        glEnableVertexAttribArray(5);
        //        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof (geo::BoneData), reinterpret_cast<GLvoid*>(16* sizeof(float)));

        //        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //        bonesB.attrib(AttriIndex{4}, AttriSize{4}, AttriType{GL_INT}, Stride{sizeof (geo::BoneData)}, AttribOffset{reinterpret_cast<GLvoid*>(0* sizeof(float))});
        //        bonesB.attrib(AttriIndex{5}, AttriSize{4}, AttriType{GL_FLOAT}, Stride{sizeof (geo::BoneData)}, AttribOffset{reinterpret_cast<GLvoid*>(16* sizeof(float))});


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

auto TriangleMeshData::render() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_triangles_with_ebo(nIndices);
    VAO::unbind();
}

auto TriangleMeshData::render_adjacency() const -> void{

    if(!buffersInitialized){
        return;
    }

    vao.bind();
    draw_triangles_adjacency_with_ebo(nIndices);
    VAO::unbind();
}

auto TriangleMeshData::clean() -> void{
    vao.clean();
    pointsB.clean();
    indicesB.clean();
    normalsB.clean();
    tangentsB.clean();
    texCoordsB.clean();
    buffersInitialized = false;
}






