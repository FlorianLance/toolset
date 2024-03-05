
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

#pragma once

// local
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl {

struct VBO{
    
    VBO() = default;
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;
    VBO(VBO&& other) = delete;
    VBO& operator=(VBO&& other) = delete;
    ~VBO();

    [[nodiscard]] constexpr auto id() const noexcept -> GLuint{return m_handle;};

    auto generate() -> void;
    auto clean() -> void;

    auto load_data(const GLfloat *data, GLsizeiptr size, GLbitfield  usage = defaultUsage) -> void;
    auto load_data(const GLint *data, GLsizeiptr size, GLbitfield  usage = defaultUsage) -> void;
    auto load_data(const GLvoid *data, GLsizeiptr size, GLbitfield  usage = defaultUsage) -> void;

    auto attrib(AttriIndex index, AttriSize size, AttriType type, Stride stride, AttribOffset offset = {nullptr}) -> bool;
    // TODO: DSA ATRIB
    auto dsa_attrib(GLuint vao, GLint index, GLint size, GLsizei stride, GLenum type, GLuint relativeoffset, GLuint bindingIndex = 0, GLintptr offset = 0, GLboolean normalized = GL_FALSE) -> void;

private:

    static inline GLbitfield  defaultUsage = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
    GLuint m_handle = 0;
};


}
