

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

#include "vao.hpp"

// base
#include "utility/logger.hpp"

using namespace tool::gl;

VAO::~VAO(){
    clean();
}

auto VAO::initialize() -> void{
    if(m_handle != 0){
        Log::warning(std::format("[VAO::generate] VAO already generated: {}\n", m_handle));
        return;
    }
    glCreateVertexArrays(1, &m_handle);
}

auto VAO::bind() const -> void{

    if(m_handle == 0){
        Log::error("[VAO::bind] VAO not generated, cannot bind it.\n");
        return;
    }

    if(currentId == m_handle){
        return;
    }

    glBindVertexArray(m_handle);
    currentId = m_handle;
}

auto VAO::unbind() -> void{

    if(currentId == 0){
        return;
    }
    glBindVertexArray(currentId = 0);
}

auto VAO::clean() -> void {

    if(m_handle == 0){
        return;
    }

    glDeleteVertexArrays(1, &m_handle);
    m_handle = 0;
}
