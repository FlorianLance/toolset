
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

#include "render_buffer_object.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

RBO::~RBO(){
    clean();
}

auto RBO::initialize() -> void{

    if(m_handle != 0){
        Logger::error(std::format("[RBO::generate] RBO already generated: {}\n", m_handle));
        return;
    }
    glCreateRenderbuffers(1, &m_handle);
}

auto RBO::clean() -> void{

    if(m_handle == 0){
        return;
    }
    GL::delete_buffers(1, &m_handle);
    m_handle = 0;
}

auto RBO::bind() -> void{
    glBindRenderbuffer(GL_RENDERBUFFER, m_handle);
}

auto RBO::unbind() -> void{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

auto RBO::set_multisample_data_storage(GLsizei width, GLsizei height, GLsizei samples) -> void{
    glNamedRenderbufferStorageMultisample(m_handle, samples, GL_DEPTH24_STENCIL8, width, height);
}

auto RBO::set_data_storage(GLsizei width, GLsizei height) -> void{
    glNamedRenderbufferStorage(m_handle, GL_DEPTH24_STENCIL8, width, height);
    // glNamedRenderbufferStorage(m_id, GL_DEPTH_COMPONENT, width, height);
    // GL_DEPTH_COMPONENT16 	GL_DEPTH_COMPONENT 	16
    // GL_DEPTH_COMPONENT24 	GL_DEPTH_COMPONENT 	24
    // GL_DEPTH_COMPONENT32F 	GL_DEPTH_COMPONENT 	f32
    // GL_DEPTH24_STENCIL8 	GL_DEPTH_STENCIL 	24 8
    // GL_DEPTH32F_STENCIL8 	GL_DEPTH_STENCIL 	f32 8
    // GL_STENCIL_INDEX8       GL_STENCIL          8
}
