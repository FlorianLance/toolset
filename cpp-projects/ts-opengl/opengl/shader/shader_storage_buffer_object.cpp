
/*******************************************************************************
** Toolset-ts-opengl                                                          **
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

#include "shader_storage_buffer_object.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

SSBO::~SSBO(){
    clean();
}

auto SSBO::generate() -> void{

    if(m_handle != 0){
        Log::error(std::format("[SSBO::generate] SSBO already generated [{}]\n", m_handle));
        return;
    }
    GL::create_buffers(1, &m_handle);
}

auto SSBO::clean() -> void {

    if(m_handle == 0){
        return;
    }
    GL::delete_buffers(1, &m_handle);
    m_handle = 0;
}

auto SSBO::bind_to_index(GLuint index) const -> bool{

    if(m_handle == 0){
        Log::error("[SSBO::bind_to_index] SSBO not generated, cannot bind it.\n");
        return false;
    }

    GL::bind_buffer_range(
        GL_SHADER_STORAGE_BUFFER,
        index,
        m_handle,
        0,
        1
    );

    return true;
}

auto SSBO::set_data_storage(GLsizei size, GLenum usage) -> void{

    glNamedBufferStorage(
        m_handle,       // GLuint buffer,
        size,       // GLsizeiptr size,
        nullptr,    // const void *data,
        usage       // GLbitfield flags
    );
}
