
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

#include "element_buffer_object.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

EBO::~EBO(){
    clean();
}

auto EBO::generate() -> void{

    if(m_id != 0){
        Logger::error(std::format("[EBO::generate] EBO already generated: {}\n", m_id));
        return;
    }
    GL::create_buffers(1, &m_id);
}

auto EBO::clean() -> void{

    if(m_id == 0){
        return;
    }
    GL::delete_buffers(1, &m_id);
    m_id = 0;
}

auto EBO::bind() -> void{

    if(m_id == 0){
        Logger::error("[GL] EBO not generated, cannot bind it.\n");
        return;
    }

    GL::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

auto EBO::unbind() -> void{
    GL::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

auto EBO::load_data(const GLuint *data, GLsizeiptr size, GLbitfield usage) -> void{

    GL::named_buffer_storage(
        m_id,
        size,
        data,
        usage
    );
}
