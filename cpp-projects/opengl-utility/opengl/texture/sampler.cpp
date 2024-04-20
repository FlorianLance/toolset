
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


#include "sampler.hpp"

// base
#include "utility/logger.hpp"


// local
#include "opengl/texture/texture_buffer_object.hpp"
#include "opengl/gl_functions.hpp"

using namespace tool::gl;


Sampler::~Sampler(){
    clean();
}

auto Sampler::initialize() -> void{

    if(is_initialized()){
        Logger::error(std::format("[Sampler::initialize] Already initialized (id:{}).\n", m_handle));
        return;
    }

    GL::create_samplers(1, &m_handle);
}

auto Sampler::clean() -> void{

    if(!is_initialized()){
        return;
    }

    GL::delete_samplers(1, &m_handle);
    m_handle         = 0;

}

auto Sampler::update(const graphics::TextureOptions &textureOptions) -> bool{

    if(!is_initialized()){
        Logger::error("[Sampler::update] Not initialized.\n");
        return false;
    }

    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_S,     to_gl(textureOptions.wrapS));
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_T,     to_gl(textureOptions.wrapT));
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_R,     to_gl(textureOptions.wrapR));
    glSamplerParameteri(m_handle, GL_TEXTURE_MIN_FILTER, to_gl(textureOptions.minFilter));
    glSamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, to_gl(textureOptions.magFilter));
    glSamplerParameterf(m_handle, GL_TEXTURE_MIN_LOD,    textureOptions.minLOD);
    glSamplerParameterf(m_handle, GL_TEXTURE_MAX_LOD,    textureOptions.maxLOD);

    glSamplerParameterfv(m_handle, GL_TEXTURE_BORDER_COLOR, textureOptions.borderColor.array.data());

    // GL_TEXTURE_WRAP_S
    // GL_TEXTURE_WRAP_T
    // GL_TEXTURE_WRAP_R
    // GL_TEXTURE_MIN_FILTER
    // GL_TEXTURE_MAG_FILTER
    // GL_TEXTURE_BORDER_COLOR

    // GL_TEXTURE_MIN_LOD, GL_TEXTURE_MAX_LOD
    // GL_TEXTURE_LOD_BIAS
    // GL_TEXTURE_COMPARE_MODE
    // GL_TEXTURE_COMPARE_FUNC.



    return true;
}

auto Sampler::bind(GLuint unit) const -> void{
    GL::bind_samplers(unit, 1, &m_handle);
}

auto Sampler::bind(std::span<const Sampler> samplers, GLuint first) -> void{
    GL::bind_samplers(first, static_cast<GLsizei>(samplers.size()), &samplers.data()->m_handle);
}

auto Sampler::unbind(GLuint first, GLuint count) -> void{
    GL::bind_samplers(first, count, nullptr);
}

