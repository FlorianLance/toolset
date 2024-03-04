
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

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

VBO::~VBO(){
    clean();
}

auto VBO::generate() -> void{

    if(m_handle != 0){
        Logger::error(std::format("[VBO::generate] VBO already generated: {}.\n", m_handle));
        return;
    }
    GL::create_buffers(1, &m_handle);
}

auto VBO::clean() -> void{

    if(m_handle == 0){
        return;
    }

    GL::delete_buffers(1, &m_handle);
    m_handle = 0;
}


auto VBO::load_data(const GLint *data, GLsizeiptr size, GLbitfield  usage) -> void{
    GL::named_buffer_storage(
        m_handle,
        size,
        data,
        usage
    );
}

auto VBO::load_data(const GLfloat *data, GLsizeiptr size, GLbitfield  usage) -> void{
    GL::named_buffer_storage(
        m_handle,
        size,
        data,
        usage
    );
}


auto VBO::attrib(AttriIndex index, AttriSize size, AttriType type, Stride stride, AttribOffset offset) -> bool{

    GL::bind_buffer(GL_ARRAY_BUFFER, m_handle);
    GL::enable_vertex_attrib_array(index.v);

    if(type.v == GL_INT){
        GL::vertex_attrib_i_pointer(index.v, size.v, type.v, stride.v,  offset.v);
    }else{
        GL::vertex_attrib_pointer(index.v, size.v, type.v, GL_FALSE, stride.v,  offset.v);
    }

    GL::bind_buffer(GL_ARRAY_BUFFER, 0);

    return true;
}

auto VBO::dsa_attrib(GLuint vao, GLint index, GLint size, GLsizei stride, GLenum type, GLuint relativeoffset, GLuint bindingIndex, GLintptr offset, GLboolean normalized) -> void{

    GL::vertex_array_vertex_buffer(
        vao,
        bindingIndex,
        m_handle,
        offset,
        stride
    );

    GL::enable_vertex_array_attrib(vao,index);

    GL::vertex_array_attrib_format(
        vao,
        index,
        size,
        type,
        normalized,
        relativeoffset
    );

    GL::vertex_array_attrib_binding(
        vao,
        index,          // GLuint attribindex: The index of the attribute to associate with a vertex buffer binding.
        bindingIndex    // GLuint bindingindex: The index of the vertex buffer binding with which to associate the generic vertex attribute.
    );

}


