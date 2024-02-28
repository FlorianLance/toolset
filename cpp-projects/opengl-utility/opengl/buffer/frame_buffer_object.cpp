
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

#include "frame_buffer_object.hpp"

// base
#include "utility/logger.hpp"

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

FrameBufferObject::~FrameBufferObject(){
    clean();
}

auto FrameBufferObject::generate() -> void{

    if(m_handle != 0){
        Logger::error(std::format("[FBO::generate] FBO already generated: {}\n", m_handle));
        return;
    }
    glCreateFramebuffers(1, &m_handle);

    static GLint maximumColorAttachmentsCount = 0;
    if(maximumColorAttachmentsCount == 0){
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maximumColorAttachmentsCount);
    }
    colorAttachments.resize(maximumColorAttachmentsCount);
}

auto FrameBufferObject::clean() -> void{

    if(m_handle == 0){
        return;
    }
    glDeleteBuffers(1, &m_handle);
    m_handle = 0;
}

auto FrameBufferObject::bind() const -> void{
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

auto FrameBufferObject::unbind() -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto FrameBufferObject::attach_depth_texture(const TBO &depthTexture, GLint mipmapLevel) -> void{

    glNamedFramebufferTexture(
        m_handle,
        GL_DEPTH_ATTACHMENT,
        // attachment: attachment specifies the logical attachment of the framebuffer and must be
        //      GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or GL_DEPTH_STENCIL_ATTACHMENT
        //      GL_COLOR_ATTACHMENTi may range from zero to the value of GL_MAX_COLOR_ATTACHMENTS minus one.
        //      Attaching a level of a texture to GL_DEPTH_STENCIL_ATTACHMENT is equivalent to attaching that
        //      level to both the GL_DEPTH_ATTACHMENT and the GL_STENCIL_ATTACHMENT attachment points simultaneously.
        m_depthAttachment = depthTexture.id(), // texture: Specifies the name of an existing texture object to attach.
        mipmapLevel   // level: Specifies the mipmap level of the texture object to attach.
    );
}

auto FrameBufferObject::attach_depth_buffer(const RBO &depthBuffer) -> void{

    // attach a renderbuffer as a logical buffer of a framebuffer object
    glNamedFramebufferRenderbuffer(
        m_handle,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        m_depthAttachment = depthBuffer.id()
    );
}

auto FrameBufferObject::attach_color0_texture(const TBO &color0Texture, GLint mipmapLevel) -> void{

    // attach a level of a texture object as a logical buffer of a framebuffer object
    glNamedFramebufferTexture(
        m_handle,
        GL_COLOR_ATTACHMENT0,
        // attachment: attachment specifies the logical attachment of the framebuffer and must be
        //      GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or GL_DEPTH_STENCIL_ATTACHMENT
        //      GL_COLOR_ATTACHMENTi may range from zero to the value of GL_MAX_COLOR_ATTACHMENTS minus one.
        //      Attaching a level of a texture to GL_DEPTH_STENCIL_ATTACHMENT is equivalent to attaching that
        //      level to both the GL_DEPTH_ATTACHMENT and the GL_STENCIL_ATTACHMENT attachment points simultaneously.
        colorAttachments[0] = color0Texture.id(), // texture: Specifies the name of an existing texture object to attach.
        mipmapLevel   // level: Specifies the mipmap level of the texture object to attach.
    );
}

auto FrameBufferObject::attach_color1_texture(const TBO &color1Texture, GLint mipmapLevel ) -> void{

    glNamedFramebufferTexture(
        m_handle,
        GL_COLOR_ATTACHMENT1,
        colorAttachments[1] = color1Texture.id(),
        mipmapLevel
    );
}

auto FrameBufferObject::attach_color2_texture(const TBO &color2Texture, GLint mipmapLevel) -> void{

    glNamedFramebufferTexture(
        m_handle,
        GL_COLOR_ATTACHMENT2,
        colorAttachments[2] = color2Texture.id(),
        mipmapLevel
    );
}

auto FrameBufferObject::attach_color3_texture(const TBO &color3Texture, GLint mipmapLevel) -> void{

    glNamedFramebufferTexture(
        m_handle,
        GL_COLOR_ATTACHMENT3,
        colorAttachments[3] = color3Texture.id(),
        mipmapLevel
    );
}

auto FrameBufferObject::attach_color_texture(const TBO &colorTexture, size_t id, GLint mipmapLevel) -> void{

    if(id >= colorAttachments.size()){
        Logger::error("[FBO::attach_color_texture] Id color texture attachment too large\n.");
        return;
    }

    glNamedFramebufferTexture(
        m_handle,
        colorsAttachmentsEnums[id],
        colorAttachments[id] = colorTexture.id(),
        mipmapLevel
    );
}

auto FrameBufferObject::attach_color_textures(const std::vector<TBO*> colorTextures) -> void{
    for(size_t ii = 0; ii < colorTextures.size(); ++ii){
        attach_color_texture(*colorTextures[ii], ii);
    }
}

auto FrameBufferObject::set_read_buffer(FrameBuffer mode) -> void{
    glNamedFramebufferReadBuffer(m_handle, static_cast<GLenum>(mode));
}

auto FrameBufferObject::set_draw_buffer(FrameBuffer mode) -> void{
    glNamedFramebufferDrawBuffer(m_handle, static_cast<GLenum>(mode));
}

auto FrameBufferObject::set_draw_buffers(std::vector<FrameBuffer> attachments) -> void{

    glNamedFramebufferDrawBuffers(
        m_handle,
        static_cast<GLsizei>(attachments.size()),
        reinterpret_cast<unsigned int*>(&attachments[0])
    );
}

auto FrameBufferObject::check_validity() -> bool{
    GLenum result = glCheckNamedFramebufferStatus(m_handle, GL_FRAMEBUFFER);
    if(result == GL_FRAMEBUFFER_COMPLETE) {
        return true;
    }
    Logger::error(std::format("[FBO::check_validity] Framebuffer error: {}\n", result));
    return false;
}

