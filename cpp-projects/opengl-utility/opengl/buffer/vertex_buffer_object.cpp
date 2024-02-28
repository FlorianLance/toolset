
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

#include "vertex_buffer_object.hpp"

// base
#include "utility/logger.hpp"

using namespace tool::gl;

VertexBufferObject::~VertexBufferObject(){
    clean();
}

auto VertexBufferObject::generate() -> void{

    if(m_handle != 0){
        Logger::error(std::format("VBO already generated: {}.\n", m_handle));
        return;
    }
    glCreateBuffers(1, &m_handle);
}

auto VertexBufferObject::clean() -> void{

    if(m_handle == 0){
        return;
    }

    glDeleteBuffers(1, &m_handle);
    m_handle = 0;
}


auto VertexBufferObject::load_data(const GLint *data, GLsizeiptr size, GLenum usage) -> void{
    glNamedBufferStorage(
        m_handle,   // GLuint buffer
        size,       // GLsizeiptr size
        data,       // const void *data
        usage       // GLenum usage
    );
}

auto VertexBufferObject::load_data(const GLfloat *data, GLsizeiptr size, GLenum usage) -> void{
    glNamedBufferStorage(
        m_handle,   // GLuint buffer
        size,       // GLsizeiptr size
        data,       // const void *data
        usage       // GLenum usage
    );
}


auto VertexBufferObject::attrib(AttriIndex index, AttriSize size, AttriType type, Stride stride, AttribOffset offset) -> bool{

    glBindBuffer(GL_ARRAY_BUFFER, m_handle);
    glEnableVertexAttribArray(index.v);

    if(type.v == GL_INT){
        glVertexAttribIPointer(index.v, size.v, type.v, stride.v,  offset.v);
    }else{
        glVertexAttribPointer(index.v, size.v, type.v, GL_FALSE, stride.v,  offset.v);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

auto VertexBufferObject::dsa_attrib(GLuint vao, GLint index, GLint size, GLsizei stride, GLenum type, GLuint relativeoffset, GLuint bindingIndex, GLintptr offset, GLboolean normalized) -> void{

    glVertexArrayVertexBuffer(
        vao,            // GLuint vaobj: Specifies the name of the vertex array object to be used by glVertexArrayVertexBuffer function.
        bindingIndex,   // GLuint bindingindex: The index of the vertex buffer binding point to which to bind the buffer.
        m_handle,       // GLuint buffer: The name of a buffer to bind to the vertex buffer binding point.
        offset,         // GLintptr offset: The offset of the first element of the buffer.
        stride          // GLsizei stride: The distance between elements within the buffer.
    );

    glEnableVertexArrayAttrib(
        vao,
        index // GLuint index: Specifies the index of the generic vertex attribute to be enabled or disabled.
    );

    glVertexArrayAttribFormat(
        vao,            // GLuint vaobj: Specifies the name of the vertex array object for glVertexArrayAttrib{I, L}Format functions.
        index,          // GLuint attribindex: The generic vertex attribute array being described.
        size,           // GLint size: The number of values per vertex that are stored in the array.
        type,           // GLenum type: The type of the data stored in the array.
        normalized,     // GLboolean normalized: Specifies whether fixed-point data values should be normalized (GL_TRUE) or converted directly as fixed-point values (GL_FALSE) when they are accessed. This parameter is ignored if type is GL_FIXED.
        relativeoffset  // GLuint relativeoffset: The distance between elements within the buffer.
    );

    glVertexArrayAttribBinding(
        vao,
        index,          // GLuint attribindex: The index of the attribute to associate with a vertex buffer binding.
        bindingIndex    // GLuint bindingindex: The index of the vertex buffer binding with which to associate the generic vertex attribute.
    );

    // example
    // glVertexArrayVertexBuffer(m_VAO, 0, m_Buffers[VERTEX_BUFFER], 0, sizeof(Vertex));
    // size_t NumFloats = 0;
    // glEnableVertexArrayAttrib(m_VAO, POSITION_LOCATION);
    // glVertexArrayAttribFormat(m_VAO, POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, (GLuint)(NumFloats * sizeof(float)));
    // glVertexArrayAttribBinding(m_VAO, POSITION_LOCATION, 0);
    // NumFloats += 3;
}


