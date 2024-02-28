
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

// local
#include "opengl/texture/texture_buffer_object.hpp"

using namespace tool::gl;

Sampler::Sampler(const graphics::TextureOptions &textureOptions){
    glCreateSamplers(1, &m_id);
    initialize(textureOptions);
}

auto Sampler::generate(const std::vector<graphics::TextureOptions> &texturesOptions) -> std::vector<tool::gl::Sampler>{

    std::vector<Sampler> samplers;
    samplers.resize(texturesOptions.size());
    glCreateSamplers(static_cast<GLsizei>(texturesOptions.size()), &samplers[0].m_id);

    for(size_t ii = 0; ii < texturesOptions.size(); ++ii){
        samplers[ii].initialize(texturesOptions[ii]);
    }

    return samplers;
}

auto Sampler::initialize(const graphics::TextureOptions &textureOptions) -> void{
    glSamplerParameteri(m_id, GL_TEXTURE_WRAP_S,     to_gl(textureOptions.wrapS));
    glSamplerParameteri(m_id, GL_TEXTURE_WRAP_T,     to_gl(textureOptions.wrapT));
    glSamplerParameteri(m_id, GL_TEXTURE_WRAP_R,     to_gl(textureOptions.wrapR));
    glSamplerParameteri(m_id, GL_TEXTURE_MIN_FILTER, to_gl(textureOptions.minFilter));
    glSamplerParameteri(m_id, GL_TEXTURE_MAG_FILTER, to_gl(textureOptions.magFilter));
    glSamplerParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, textureOptions.borderColor.array.data());
}

auto Sampler::bind(GLuint unitIndex) const -> void{
    glBindSampler(unitIndex, m_id);
}

auto Sampler::bind_samplers(const std::vector<Sampler> &samplers, GLuint first) -> void{
    glBindSamplers(first, static_cast<GLsizei>(samplers.size()), &samplers.data()->m_id);
}

auto Sampler::unbind(GLuint first, GLuint count) -> void{
    glBindSamplers(first, count, nullptr);
}


//// sampler objects
//GLuint samplers[2];
//glCreateSamplers(2, samplers);

//linearSampler = samplers[0];
//glSamplerParameteri(linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//glSamplerParameteri(linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//nearestSampler = samplers[1];
//glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST)


//// Set up two sampler objects for linear and nearest filtering
//GLuint samplers[2];
//glGenSamplers(2, samplers);
//linearSampler = samplers[0];
//nearestSampler = samplers[1];

//GLfloat border[] = {0.0f,0.0f,0.0f,0.0f};
//// Set up the nearest sampler
//glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//glSamplerParameterfv(nearestSampler, GL_TEXTURE_BORDER_COLOR, border);

//// Set up the linear sampler
//glSamplerParameteri(linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//glSamplerParameteri(linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//glSamplerParameterfv(linearSampler, GL_TEXTURE_BORDER_COLOR, border);

//// We want nearest sampling except for the last pass.
//glBindSampler(0, nearestSampler);
//glBindSampler(1, nearestSampler);
//glBindSampler(2, nearestSampler);
