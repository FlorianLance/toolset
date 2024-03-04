
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

// glew
#include <GL/glew.h>

namespace tool::gl{

struct ABO{

    ABO() = default;
    ABO(const ABO&) = delete;
    ABO& operator=(const ABO&) = delete;
    ABO(ABO&& other) = default;
    ABO& operator=(ABO&& other) = default;
    ~ABO();

    auto generate() -> void;
    auto clean() -> void;

    [[nodiscard]] constexpr auto id() const noexcept -> GLuint{return m_handle;}

    auto bind_to_index(GLuint index) const -> bool;

    auto set_data_storage(GLsizeiptr sizeData, GLenum usage = 0) -> void;
    auto update_data(GLuint *data, GLsizeiptr sizeData, GLintptr offset = 0) -> void;

private:

    GLuint m_handle = 0;
    GLsizeiptr m_size = 0;
};
}

