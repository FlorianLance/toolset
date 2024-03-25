
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
#include "gl/glew.h"

namespace tool::gl{

struct EBO{

    EBO() = default;
    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;
    EBO(EBO&& other) = default;
    EBO& operator=(EBO&& other) = default;
    ~EBO();

    auto generate() -> void;
    auto clean() -> void;

    auto load_data(const GLuint *data, GLsizeiptr size, GLbitfield usage = defaultUsage) -> void;
    auto update_data(const GLuint *data, GLsizeiptr size, GLintptr offset) -> void;


    auto bind() -> void;
    static auto unbind() -> void;


private:

    static inline GLbitfield  defaultUsage = 0;
    GLuint m_handle = 0;
};


}


