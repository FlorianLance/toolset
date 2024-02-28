
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

// local
#include "opengl/texture/texture_buffer_object.hpp"
#include "render_buffer_object.hpp"

namespace tool::gl{

using namespace std::literals::string_view_literals;

enum class FrameBuffer : unsigned int{
    None        = GL_NONE,
    Front_left  = GL_FRONT_LEFT,
    Front_right = GL_FRONT_RIGHT,
    Back_left   = GL_BACK_LEFT,
    Back_right  = GL_BACK_RIGHT,
    Color0      = GL_COLOR_ATTACHMENT0,
    Color1      = GL_COLOR_ATTACHMENT1,
    Color2      = GL_COLOR_ATTACHMENT2,
    Color3      = GL_COLOR_ATTACHMENT3,
    Color4      = GL_COLOR_ATTACHMENT4,
    Color5      = GL_COLOR_ATTACHMENT5,
    Color6      = GL_COLOR_ATTACHMENT6,
    Color7      = GL_COLOR_ATTACHMENT7,
    Color8      = GL_COLOR_ATTACHMENT8,
    Color9      = GL_COLOR_ATTACHMENT9,
    Color10     = GL_COLOR_ATTACHMENT10,
    Color11     = GL_COLOR_ATTACHMENT11,
    Color12     = GL_COLOR_ATTACHMENT12,
    Color13     = GL_COLOR_ATTACHMENT13,
    Color14     = GL_COLOR_ATTACHMENT14,
    Color15     = GL_COLOR_ATTACHMENT15,
};

struct FrameBufferObject{

    FrameBufferObject() = default;
    FrameBufferObject(const FrameBufferObject&) = delete;
    FrameBufferObject& operator=(const FrameBufferObject&) = delete;
    FrameBufferObject(FrameBufferObject&& other) = default;
    FrameBufferObject& operator=(FrameBufferObject&& other) = default;
    ~FrameBufferObject();

    auto generate() -> void;
    auto clean() -> void;

    [[nodiscard]] constexpr auto id() const noexcept -> GLuint{return m_handle;}
    [[nodiscard]] constexpr auto color_attachment(size_t id = 0) const noexcept -> GLuint {return colorAttachments[id];}
    [[nodiscard]] constexpr auto depth_attachment() const noexcept -> GLuint{return m_depthAttachment;}

    auto bind() const -> void;
    static auto unbind() -> void;

    auto attach_depth_texture(const TBO &depthTexture, GLint mipmapLevel = 0) -> void;
    auto attach_depth_buffer(const RBO &depthBuffer) -> void;

    auto attach_color0_texture(const TBO &color0Texture, GLint mipmapLevel = 0) -> void;
    auto attach_color1_texture(const TBO &color1Texture, GLint mipmapLevel = 0) -> void;
    auto attach_color2_texture(const TBO &color2Texture, GLint mipmapLevel = 0) -> void;
    auto attach_color3_texture(const TBO &color3Texture, GLint mipmapLevel = 0) -> void;
    auto attach_color_texture(const TBO &colorTexture, size_t id, GLint mipmapLevel = 0) -> void;
    auto attach_color_textures(const std::vector<TBO*> colorTextures) -> void;

    auto set_read_buffer(FrameBuffer mode) -> void;
    auto set_draw_buffer(FrameBuffer mode) -> void;
    auto set_draw_buffers(std::vector<FrameBuffer> attachments) -> void;

    auto check_validity() -> bool;

private:

    GLuint m_handle = 0;
    std::vector<GLuint> colorAttachments = {0};
    GLuint m_depthAttachment = 0;
};
}


