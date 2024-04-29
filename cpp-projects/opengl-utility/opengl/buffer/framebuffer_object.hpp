
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
    None        = GL_NONE,                  // The fragment shader output value is not written into any color buffer.
    Front_left  = GL_FRONT_LEFT,            // The fragment shader output value is written into the front left color buffer.
    Front_right = GL_FRONT_RIGHT,           // The fragment shader output value is written into the front right color buffer.
    Back_left   = GL_BACK_LEFT,             // The fragment shader output value is written into the back left color buffer.
    Back_right  = GL_BACK_RIGHT,            // The fragment shader output value is written into the back right color buffer.
    Color0      = GL_COLOR_ATTACHMENT0,     // The fragment shader output value is written into the nth color attachment of the current framebuffer.
                                            // n may range from zero to the value of GL_MAX_COLOR_ATTACHMENTS.
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

struct FBO{

    FBO() = default;
    FBO(const FBO&) = delete;
    FBO& operator=(const FBO&) = delete;
    FBO(FBO&& other) = delete;
    FBO& operator=(FBO&& other) = delete;
    ~FBO();

    auto initialize() -> void;
    auto clean() -> void;

    [[nodiscard]] constexpr auto id() const noexcept -> GLuint{return m_handle;}

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
};
}


