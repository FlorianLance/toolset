
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

#pragma once

// std
#include <span>

// base
#include "image/texture_2d.hpp"
#include "image/texture_options.hpp"

// local
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl{


enum class TextureFormat : std::int8_t{
    red,                            // Each element is a single red component. For fixed point normalized components, the GL converts it to floating point, clamps to the range [0,1], and assembles it into an RGBA element by attaching 0.0 for green and blue, and 1.0 for alpha.
    red_integer,                    // Each element is a single red component. The GL performs assembles it into an RGBA element by attaching 0 for green and blue, and 1 for alpha.
    red_green,                      // Each element is a red/green double. For fixed point normalized components, the GL converts each component to floating point, clamps to the range [0,1], and assembles them into an RGBA element by attaching 0.0 for blue, and 1.0 for alpha.
    red_green_integer,              // Each element is a red/green double. The GL assembles them into an RGBA element by attaching 0 for blue, and 1 for alpha.
    red_green_blue,                 // Each element is an RGB triple. For fixed point normalized components, the GL converts each component to floating point, clamps to the range [0,1], and assembles them into an RGBA element by attaching 1.0 for alpha.
    red_green_blue_integer,         // Each element is an RGB triple. The GL assembles them into an RGBA element by attaching 1 for alpha.
    red_green_blue_alpha,           // Each element contains all four components. For fixed point normalized components, the GL converts each component to floating point and clamps them to the range [0,1].
    red_green_blue_alpha_integer,   // Each element contains all four components.
    depth_component,                // Each element is a single depth value. The GL converts it to floating point, and clamps to the range [0,1].
    depth_stencil,                  // Each element is a pair of depth and stencil values. The depth component of the pair is interpreted as in GL_DEPTH_COMPONENT. The stencil component is interpreted based on specified the depth + stencil internal format.
    luminance,                      // Each element is a single luminance component. The GL converts it to floating point, clamps to the range [0,1], and assembles it into an RGBA element by placing the luminance value in the red, green and blue channels, and attaching 1.0 to the alpha channel.
    luminance_alpha,                // Each element is an luminance/alpha double. The GL converts each component to floating point, clamps to the range [0,1], and assembles them into an RGBA element by placing the luminance value in the red, green and blue channels.
    alpha,                          // Each element is a single alpha component. The GL converts it to floating point, clamps to the range [0,1], and assembles it into an RGBA element by placing attaching 0.0 to the red, green and blue channels.
    SizeEnum,
    R       = red,
    RG      = red_green,
    RGB     = red_green_blue,
    RGBA    = red_green_blue_alpha,
    R_I     = red_integer,
    RG_I    = red_green_integer,
    RGB_I   = red_green_blue_integer,
    RGBA_I  = red_green_blue_alpha_integer,
    DC      = depth_component,
    DS      = depth_stencil,
    L       = luminance,
    LA      = luminance_alpha
};

enum class TextureDataType{
    unsigned_byte_t,
    byte_t,
    unsigned_short_t,
    short_t,
    unsigned_int_t,
    int_t,
    half_float_t,
    float_t,
    SizeEnum
};

using TTextureDataTypeGl = std::tuple<
    TextureDataType,     unsigned int>;
static constexpr TupleArray<TextureDataType::SizeEnum, TTextureDataTypeGl> textureDataTypesGl ={{
    TTextureDataTypeGl
    {TextureDataType::unsigned_byte_t,  GL_UNSIGNED_BYTE},
    {TextureDataType::byte_t,           GL_BYTE},
    {TextureDataType::unsigned_short_t, GL_UNSIGNED_SHORT},
    {TextureDataType::short_t,          GL_SHORT},
    {TextureDataType::unsigned_int_t,   GL_UNSIGNED_INT},
    {TextureDataType::int_t,            GL_INT},
    {TextureDataType::half_float_t,     GL_HALF_FLOAT},
    {TextureDataType::float_t,          GL_FLOAT}
}};
[[maybe_unused]] static constexpr auto to_gl(TextureDataType t) -> unsigned int{
    return textureDataTypesGl.at<0,1>(t);
}


using TextureType       = img::TextureType;
using TextureOptions    = img::TextureOptions;

using TextureMinFilter = img::TextureMinFilter;
using TMinFiltGl = std::tuple<
    img::TextureMinFilter,                 unsigned int>;
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

using TextureMagFilter = img::TextureMagFilter;
using TMagFiltGl = std::tuple<
    img::TextureMagFilter,             unsigned int>;
static constexpr TupleArray<TextureMagFilter::SizeEnum, TMagFiltGl> textureMagFiltersGl ={{
    TMagFiltGl
    {TextureMagFilter::nearest,             GL_NEAREST},
    {TextureMagFilter::linear,              GL_LINEAR},
}};

[[maybe_unused]] static constexpr auto to_gl(TextureMagFilter t) -> unsigned int{
    return textureMagFiltersGl.at<0,1>(t);
}

using TextureMode = img::TextureMode;
using TTexModeGl = std::tuple<
    img::TextureMode,                      GLenum>;
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

using TextureWrapMode = img::TextureWrapMode;
using TTexWrapModeGl = std::tuple<
    img::TextureWrapMode,              unsigned int>;
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

using TTextureFormatGl = std::tuple<
    TextureFormat,     unsigned int>;
static constexpr TupleArray<TextureFormat::SizeEnum, TTextureFormatGl> textureFormatsGl ={{
    TTextureFormatGl
    {TextureFormat::red,                            GL_RED},
    {TextureFormat::red_integer,                    GL_RED_INTEGER},
    {TextureFormat::red_green,                      GL_RG},
    {TextureFormat::red_green_blue,                 GL_RG_INTEGER},
    {TextureFormat::red_green_blue_integer,         GL_RGB_INTEGER},
    {TextureFormat::red_green_blue_alpha,           GL_RGBA},
    {TextureFormat::red_green_blue_alpha_integer,   GL_RGBA_INTEGER},
    {TextureFormat::depth_component,                GL_DEPTH_COMPONENT},
    {TextureFormat::depth_stencil,                  GL_DEPTH_STENCIL},
    {TextureFormat::luminance,                      GL_LUMINANCE},
    {TextureFormat::luminance_alpha,                GL_LUMINANCE_ALPHA},
    {TextureFormat::alpha,                          GL_ALPHA},

}};

[[maybe_unused]] static constexpr auto to_gl(TextureFormat t) -> unsigned int{
    return textureFormatsGl.at<0,1>(t);
}



struct TBO{

    TBO() = default;
    TBO(TextureMode mode) : m_mode(mode){}
    TBO(const TBO&) = delete;
    TBO& operator=(const TBO&) = delete;
    TBO(TBO&& other) = delete;
    TBO& operator=(TBO&& other) = delete;
    ~TBO();

    [[nodiscard]] constexpr auto is_initialized()   const noexcept -> bool          {return m_handle != 0;}
    [[nodiscard]] constexpr auto id()               const noexcept -> GLuint        {return m_handle;}
    [[nodiscard]] constexpr auto nb_channels()      const noexcept -> GLsizei       {return m_nbChannels;}
    [[nodiscard]] constexpr auto width()            const noexcept -> GLsizei       {return m_width;}
    [[nodiscard]] constexpr auto height()           const noexcept -> GLsizei       {return m_height;}
    [[nodiscard]] constexpr auto depth()            const noexcept -> GLsizei       {return m_depth;}
    [[nodiscard]] constexpr auto mode()             const noexcept -> TextureMode   {return m_mode;}

    auto initialize() -> void;
    auto clean() -> void;

    // init
    auto init_data(GLsizei width, GLsizei height, GLsizei depth, TextureFormat format, TextureDataType dataType, int levels = 0) -> void;

    auto init_data_u8(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_data_u32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_data_f16(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_data_f32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels = 0) -> void;
    auto init_multisample_data_u8(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels = 0) -> void;
    auto init_multisample_data_f16(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels = 0) -> void;
    auto init_multisample_data_f32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels = 0) -> void;

    // options
    auto set_texture_options(const TextureOptions &options) -> void;

    // bind / unbind
    auto bind(GLuint unit) -> void;
    // static auto bind(std::span<const GLuint> textures, GLuint first) -> void;
    static auto bind(std::vector<GLuint> textures, GLuint first) -> void;
    auto bind_image(GLuint unit, GLint level = 0, GLboolean layered = GL_FALSE, GLint layer = 0, GLenum access = GL_READ_WRITE) -> void;
    // static auto bind_image_textures(std::vector<GLuint> textures, GLuint first = 0) -> bool; // TODO: TEST
    static auto unbind_textures(GLuint first, GLuint count) -> void;

    // get
    auto get_hdr_texture_data(std::vector<GLfloat> &data) -> void;

protected:

    // update
    auto update_data(const GLubyte *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;
    auto update_data(const GLuint *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;
    auto update_data(const GLfloat *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;
    // mipmap
    auto generate_mipmap() -> void;

    GLuint m_handle = 0;

    GLenum m_internalFormat=0;
    GLenum m_format=0;
    GLenum m_type=0;

    int m_nbChannels=0;
    GLsizei m_width=0;
    GLsizei m_height=0;
    GLsizei m_depth=0;
    int m_levelsNb = 1;

private:

    auto texture_storage() -> void;
    auto texture_sub_image(const void *data, GLint level, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void;

    TextureMode m_mode;
};
}


//    void gl_texture_multisample_storage(GLsizei samples = 4, GLboolean fixedsamplelocations = GL_TRUE);
//    void gl_texture_multisample_storage_2d(GLsizei samples = 4, GLboolean fixedsamplelocations = GL_TRUE);
//    void gl_texture_multisample_storage_3d(GLsizei samples = 4, GLboolean fixedsamplelocations = GL_TRUE);
