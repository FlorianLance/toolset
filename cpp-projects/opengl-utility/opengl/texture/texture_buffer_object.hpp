
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

// std
#include <span>

// base
#include "graphics/texture/texture_2d.hpp"
#include "graphics/texture/texture_options.hpp"

// local
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl{

using TextureType       = graphics::TextureType;
using TextureOptions    = graphics::TextureOptions;

using TextureMinFilter = graphics::TextureMinFilter;
using TMinFiltGl = std::tuple<
    graphics::TextureMinFilter,                 unsigned int>;
static constexpr TupleArray<TextureMinFilter::SizeEnum, TMinFiltGl> textureMinFiltersGl ={{
    TMinFiltGl
    {TextureMinFilter::nearest,                 GL_NEAREST},
    {TextureMinFilter::linear,                  GL_LINEAR},
    {TextureMinFilter::nearest_mipmap_nearest,  GL_NEAREST_MIPMAP_NEAREST},
    {TextureMinFilter::linear_mimmap_nearest,   GL_LINEAR_MIPMAP_NEAREST},
    {TextureMinFilter::nearest_mipmap_linear,   GL_NEAREST_MIPMAP_LINEAR},
    {TextureMinFilter::linear_mipmap_linear,    GL_LINEAR_MIPMAP_LINEAR},
}};

[[maybe_unused]] static constexpr auto to_gl(TextureMinFilter t) -> unsigned int{
    return textureMinFiltersGl.at<0,1>(t);
}

using TextureMagFilter = graphics::TextureMagFilter;
using TMagFiltGl = std::tuple<
    graphics::TextureMagFilter,             unsigned int>;
static constexpr TupleArray<TextureMagFilter::SizeEnum, TMagFiltGl> textureMagFiltersGl ={{
    TMagFiltGl
    {TextureMagFilter::nearest,             GL_NEAREST},
    {TextureMagFilter::linear,              GL_LINEAR},
}};

[[maybe_unused]] static constexpr auto to_gl(TextureMagFilter t) -> unsigned int{
    return textureMagFiltersGl.at<0,1>(t);
}

using TextureMode = graphics::TextureMode;
using TTexModeGl = std::tuple<
    graphics::TextureMode,                      GLenum>;
static constexpr TupleArray<TextureMode::SizeEnum, TTexModeGl> textureModesGl ={{
    TTexModeGl
    {TextureMode::texture_1d,                   GL_TEXTURE_1D},
    {TextureMode::texture_2d,                   GL_TEXTURE_2D},
    {TextureMode::texture_3d,                   GL_TEXTURE_3D},
    {TextureMode::rectangle,                    GL_TEXTURE_RECTANGLE},
    {TextureMode::buffer,                       GL_TEXTURE_BUFFER},
    {TextureMode::cubemap,                      GL_TEXTURE_CUBE_MAP},
    {TextureMode::texture_1d_array,             GL_TEXTURE_1D_ARRAY},
    {TextureMode::texture_2d_array,             GL_TEXTURE_2D_ARRAY},
    {TextureMode::cubemap_array,                GL_TEXTURE_CUBE_MAP_ARRAY},
    {TextureMode::texture_2d_multisample,       GL_TEXTURE_2D_MULTISAMPLE},
    {TextureMode::texture_2d_multisample_array, GL_TEXTURE_2D_MULTISAMPLE_ARRAY}
}};

[[maybe_unused]] static constexpr auto to_gl(TextureMode t) -> GLenum{
    return textureModesGl.at<0,1>(t);
}

using TextureWrapMode = graphics::TextureWrapMode;
using TTexWrapModeGl = std::tuple<
    graphics::TextureWrapMode,              unsigned int>;
static constexpr TupleArray<TextureWrapMode::SizeEnum, TTexWrapModeGl> textureWrapModesGl ={{
    TTexWrapModeGl
    {TextureWrapMode::clamp_to_edge,        GL_CLAMP_TO_EDGE},
    {TextureWrapMode::clamp_to_border,      GL_CLAMP_TO_BORDER},
    {TextureWrapMode::mirrored_repeat,      GL_MIRRORED_REPEAT},
    {TextureWrapMode::repeat,               GL_REPEAT},
    {TextureWrapMode::mirror_clamp_to_edge, GL_MIRROR_CLAMP_TO_EDGE},
}};

[[maybe_unused]] static constexpr auto to_gl(TextureWrapMode t) -> unsigned int{
    return textureWrapModesGl.at<0,1>(t);
}

struct TBO{

    TBO() = default;
    TBO(TextureMode mode) : m_mode(mode){}
    TBO(const TBO&) = delete;
    TBO& operator=(const TBO&) = delete;
    TBO(TBO&& other) = default;
    TBO& operator=(TBO&& other) = default;
    ~TBO();

    auto clean() -> void;

    [[nodiscard]] constexpr auto id()          const noexcept -> GLuint{return m_handle;}
    [[nodiscard]] constexpr auto nb_channels() const noexcept -> GLsizei{return m_nbChannels;}
    [[nodiscard]] constexpr auto width()       const noexcept -> GLsizei{return m_width;}
    [[nodiscard]] constexpr auto height()      const noexcept -> GLsizei{return m_height;}
    [[nodiscard]] constexpr auto depth()       const noexcept -> GLsizei{return m_depth;}

    // bind / unbind
    auto bind(GLuint unit) -> void;
    static auto bind(std::span<GLuint> textures, GLuint first) -> void;
    static auto bind(std::vector<GLuint> textures, GLuint first) -> void;

    auto bind_image(GLuint unit, GLint level = 0, GLboolean layered = GL_FALSE, GLint layer = 0, GLenum access = GL_READ_WRITE) -> void;
    // static auto bind_image_textures(std::vector<GLuint> textures, GLuint first = 0) -> bool; // TODO: TEST
    static auto unbind_textures(GLuint first, GLuint count) -> void;

    auto set_texture_options(TextureOptions options = {}) -> void;


    auto get_hdr_texture_data(std::vector<GLfloat> &data) -> void;
    auto debug_generate() -> void;

protected:

    // generate
    auto generate() -> void;
    // init
    auto init_data_u8(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_data_u32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_data_f16(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_data_f32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_multisample_data_u8(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels = 0) -> void;
    auto init_multisample_data_f16(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels = 0) -> void;
    auto init_multisample_data_f32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels = 0) -> void;
    // update
    auto update_data(GLubyte *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;
    auto update_data(GLuint *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;
    auto update_data(GLfloat *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;
    // mipmap
    auto generate_mipmap() -> void;

    GLuint m_handle = 0;

    GLenum m_internalFormat;
    GLenum m_format;
    GLenum m_type;

    int m_nbChannels=0;
    GLsizei m_width=0;
    GLsizei m_height=0;
    GLsizei m_depth=0;
    int m_levelsNb = 1;

private:

    auto specify_storage() -> void;
    auto gl_texture_sub_image(void *data, GLint level, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;

    TextureMode m_mode;
};
}


//    void gl_texture_multisample_storage(GLsizei samples = 4, GLboolean fixedsamplelocations = GL_TRUE);
//    void gl_texture_multisample_storage_2d(GLsizei samples = 4, GLboolean fixedsamplelocations = GL_TRUE);
//    void gl_texture_multisample_storage_3d(GLsizei samples = 4, GLboolean fixedsamplelocations = GL_TRUE);
