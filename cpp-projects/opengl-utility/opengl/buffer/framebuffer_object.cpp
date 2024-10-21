
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

#include "framebuffer_object.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

static constexpr std::array<GLenum, 16> colorsAttachmentsEnums{
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7,
    GL_COLOR_ATTACHMENT8,
    GL_COLOR_ATTACHMENT9,
    GL_COLOR_ATTACHMENT10,
    GL_COLOR_ATTACHMENT11,
    GL_COLOR_ATTACHMENT12,
    GL_COLOR_ATTACHMENT13,
    GL_COLOR_ATTACHMENT14,
    GL_COLOR_ATTACHMENT15
};

FBO::~FBO(){
    clean();
}

auto FBO::initialize() -> void{

    if(m_handle != 0){
        Log::error(std::format("[FBO::generate] FBO already generated: {}\n", m_handle));
        return;
    }
    GL::create_framebuffers(1, &m_handle);
}

auto FBO::clean() -> void{

    if(m_handle == 0){
        return;
    }
    GL::delete_buffers(1, &m_handle);
    m_handle = 0;
}

auto FBO::bind() const -> void{
    GL::bind_framebuffer(GL_FRAMEBUFFER, m_handle);
}

auto FBO::unbind() -> void {
    GL::bind_framebuffer(GL_FRAMEBUFFER, 0);
}

auto FBO::attach_depth_texture(const TBO &depthTexture, GLint mipmapLevel) -> void{

    GL::named_framebuffer_texture(
        m_handle,
        GL_DEPTH_ATTACHMENT,
        depthTexture.id(),
        mipmapLevel
    );
}

auto FBO::attach_depth_buffer(const RBO &depthBuffer) -> void{

    GL::named_framebuffer_renderbuffer(
        m_handle,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        depthBuffer.id()
    );
}

auto FBO::attach_color0_texture(const TBO &color0Texture, GLint mipmapLevel) -> void{

    GL::named_framebuffer_texture(
        m_handle,
        GL_COLOR_ATTACHMENT0,
        color0Texture.id(),
        mipmapLevel
    );
}

auto FBO::attach_color1_texture(const TBO &color1Texture, GLint mipmapLevel ) -> void{

    GL::named_framebuffer_texture(
        m_handle,
        GL_COLOR_ATTACHMENT1,
        color1Texture.id(),
        mipmapLevel
    );
}

auto FBO::attach_color2_texture(const TBO &color2Texture, GLint mipmapLevel) -> void{

    GL::named_framebuffer_texture(
        m_handle,
        GL_COLOR_ATTACHMENT2,
        color2Texture.id(),
        mipmapLevel
    );
}

auto FBO::attach_color3_texture(const TBO &color3Texture, GLint mipmapLevel) -> void{

    GL::named_framebuffer_texture(
        m_handle,
        GL_COLOR_ATTACHMENT3,
        color3Texture.id(),
        mipmapLevel
    );
}

auto FBO::attach_color_texture(const TBO &colorTexture, size_t id, GLint mipmapLevel) -> void{

    if(id >= colorsAttachmentsEnums.size()){
        Log::error("[FBO::attach_color_texture] Id color texture attachment too large\n.");
        return;
    }

    GL::named_framebuffer_texture(
        m_handle,
        colorsAttachmentsEnums[id],
        colorTexture.id(),
        mipmapLevel
    );
}

auto FBO::attach_color_textures(const std::vector<TBO*> colorTextures) -> void{
    for(size_t ii = 0; ii < colorTextures.size(); ++ii){
        attach_color_texture(*colorTextures[ii], ii);
    }
}

auto FBO::set_read_buffer(FrameBuffer mode) -> void{
    GL::named_framebuffer_read_buffer(m_handle, static_cast<GLenum>(mode));
}

auto FBO::set_draw_buffer(FrameBuffer mode) -> void{
    GL::named_framebuffer_draw_buffer(m_handle, static_cast<GLenum>(mode));
}

auto FBO::set_draw_buffers(std::vector<FrameBuffer> attachments) -> void{

    GL::named_framebuffer_draw_buffers(
        m_handle,
        static_cast<GLsizei>(attachments.size()),
        reinterpret_cast<unsigned int*>(&attachments[0])
    );
}

auto FBO::check_validity() -> bool{
    GLenum result = GL::check_named_framebuffer_status(m_handle, GL_FRAMEBUFFER);
    if(result == GL_FRAMEBUFFER_COMPLETE) {
        return true;
    }
    Log::error(std::format("[FBO::check_validity] Framebuffer error: {}\n", result));
    return false;
}

