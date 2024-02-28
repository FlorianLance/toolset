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
#include "texture_buffer_object.hpp"

// std
#include <format>

// base
#include "geometry/point4.hpp"
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::gl;

TBO::~TBO(){
    clean();
}

auto TBO::generate() -> void{

    if(m_id != 0){
        Logger::error(std::format("[TBO::generate] TBO already generated [{}]\n", m_id));
        return;
    }
    glCreateTextures(to_gl(m_mode), 1, &m_id);
}

auto TBO::clean() -> void{

    if(m_id == 0){
        return;
    }

    glDeleteBuffers(1, &m_id);
    m_id = 0;
}


auto TBO::bind(GLuint unit) -> void{
    // glBindTextureUnit(unit - GL_TEXTURE0, m_id);
    bind({m_id}, unit);
}

auto TBO::bind(std::vector<TextureName> textures, GLuint first) -> void{

    for(const auto &n : textures){
        if(n==0){
            Logger::error("[TBO::bind_textures) TBO not generated, cannot bind it.\n");
            return;
        }
    }

    // bind one or more named textures to a sequence of consecutive texture units
    glBindTextures(first, static_cast<GLsizei>(textures.size()), textures.data());
}

auto TBO::unbind_textures(GLuint first, GLuint count) -> void{
    std::vector<TextureName> textures(count);
    std::fill(std::begin(textures), std::end(textures), 0);
    glBindTextures(first, static_cast<GLsizei>(textures.size()), textures.data());
}


auto TBO::bind_image(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access) -> void{

    glBindImageTexture(
        unit, // GLuint unit,
        m_id, // GLuint texture,
        level, // GLint level,
        layered, // GLboolean layered,
        layer,   // GLint layer,
        access,  // GLenum access,
        m_internalFormat // GLenum format
    );
//    return bind_image_textures({m_id});
}





auto TBO::bind_image_textures(std::vector<TextureName> textures, GLuint first) -> bool{

    for(const auto &n : textures){
        if(n==0){
            Logger::error("[TBO::bind_image_textures] TBO not generated, cannot bind it.\n");
            return false;
        }
    }

    // bind one or more named texture images to a sequence of consecutive image units
    glBindImageTextures(first, static_cast<GLsizei>(textures.size()), textures.data());
    return true;
}

auto TBO::set_texture_options(TextureOptions options) -> void{
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S,     to_gl(options.wrapS));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T,     to_gl(options.wrapT));
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R,     to_gl(options.wrapR));
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, to_gl(options.minFilter));
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, to_gl(options.magFilter));
    glTextureParameteri(m_id, GL_TEXTURE_MAX_LEVEL,  options.maxLevel);
    glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, options.borderColor.array.data());
}

auto TBO::init_data_u8(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels) -> void{

    m_width          = width;
    m_height         = height;
    m_depth          = depth;
    m_type           = GL_UNSIGNED_BYTE;
    if(levels == 0){
        m_levelsNb = std::min(5, static_cast<int>(std::log2f((float)std::max(m_width, m_height))));
    }else{
        m_levelsNb = levels;
    }

    if(nbChannels == 3){
        m_format = GL_RGB;//_INTEGER;
        m_internalFormat = GL_RGB8;
    }else if(nbChannels == 4){
        m_format = GL_RGBA;//_INTEGER;
        m_internalFormat = GL_RGBA8;
    }else if(nbChannels == 1){
        m_format = GL_R;//ED_INTEGER;
        m_internalFormat = GL_R8;
    }else if(nbChannels == 2){
        m_format = GL_RG;//_INTEGER;
        m_internalFormat = GL_RG8;
    }else{
        Logger::error("[TBO::init_data_u8] Channels nb not managed.\n");
        return;
    }
    gl_texture_storage();
}

auto TBO::init_data_u32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels) -> void{

    m_width          = width;
    m_height         = height;
    m_depth          = depth;
    m_type           = GL_UNSIGNED_INT;
    if(levels == 0){
        m_levelsNb = std::min(5, static_cast<int>(std::log2f((float)std::max(m_width, m_height))));
    }else{
        m_levelsNb = levels;
    }

    if(nbChannels == 3){
        m_format =  GL_RGB_INTEGER;
        m_internalFormat = GL_RGB32UI;
    }else if(nbChannels == 4){
        m_format =  GL_RGBA_INTEGER;
        m_internalFormat = GL_RGBA32UI;
    }else if(nbChannels == 1){
        m_format = GL_RED_INTEGER;
        m_internalFormat = GL_R32UI;
    }else if(nbChannels == 2){
        m_format =  GL_RG_INTEGER;
        m_internalFormat = GL_RG32UI;
    }else{
        Logger::error("[TBO::init_data_u32] Channels nb not managed.\n");
        return;
    }

    gl_texture_storage();
}

auto TBO::init_data_f16(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels) -> void{

    m_width          = width;
    m_height         = height;
    m_depth          = depth;
    m_type           = GL_FLOAT;
    if(levels == 0){
        m_levelsNb = std::min(5, static_cast<int>(std::log2f((float)std::max(m_width, m_height))));
    }else{
        m_levelsNb = levels;
    }

    if(nbChannels == 3){
        m_format = GL_RGB;
        m_internalFormat = GL_RGB16F;
    }else if(nbChannels == 4){
        m_format = GL_RGBA;
        m_internalFormat = GL_RGBA16F;
    }else if(nbChannels == 1){
        m_format = GL_R;
        m_internalFormat = GL_R16F;
    }else if(nbChannels == 2){
        m_format = GL_RG;
        m_internalFormat = GL_RG16F;
    }else{
        Logger::error("[TBO::init_data_f16] Channels nb not managed.\n");
        return;
    }
    gl_texture_storage();
}

auto TBO::init_data_f32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, int levels) -> void{

    m_width          = width;
    m_height         = height;
    m_depth          = depth;
    m_type           = GL_FLOAT;
    if(levels == 0){
        m_levelsNb = std::min(5, static_cast<int>(std::log2f((float)std::max(m_width, m_height))));
    }else{
        m_levelsNb = levels;
    }

    if(nbChannels == 3){
        m_format = GL_RGB;
        m_internalFormat = GL_RGB32F;
    }else if(nbChannels == 4){
        m_format = GL_RGBA;
        m_internalFormat = GL_RGBA32F;
    }else if(nbChannels == 1){
        m_format = GL_R;
        m_internalFormat = GL_R32F;
    }else if(nbChannels == 2){
        m_format = GL_RG;
        m_internalFormat = GL_RG32F;
    }else{
        Logger::error("[TBO::init_data_f32] Channels nb not managed.\n");
        return;
    }
    gl_texture_storage();
}

auto TBO::init_multisample_data_u8(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels) -> void{

    m_width          = width;
    m_height         = height;
    m_depth          = depth;
    m_type           = GL_UNSIGNED_BYTE;
    if(levels == 0){
        m_levelsNb = std::min(5, static_cast<int>(std::log2f((float)std::max(m_width, m_height))));
    }else{
        m_levelsNb = levels;
    }

    if(nbChannels == 3){
        m_format = GL_RGB;
        m_internalFormat = GL_RGB8;
    }else if(nbChannels == 4){
        m_format = GL_RGBA;
        m_internalFormat = GL_RGBA8;
    }else if(nbChannels == 1){
        m_format = GL_R;
        m_internalFormat = GL_R8;
    }else if(nbChannels == 2){
        m_format = GL_RG;
        m_internalFormat = GL_RG8;
    }else{
        Logger::error("[TBO::init_multisample_data_u8] Channels nb not managed.\n");
        return;
    }

    glTextureStorage2DMultisample(
        m_id,               // GLuint texture,
        samples,            // GLsizei samples,
        m_internalFormat,   // GLenum internalformat,
        m_width,            // GLsizei width,
        m_height,           // GLsizei height,
        GL_TRUE             // GLboolean fixedsamplelocations
    );
}

auto TBO::init_multisample_data_f16(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels) -> void{

    m_width          = width;
    m_height         = height;
    m_depth          = depth;
    m_type           = GL_FLOAT;
    if(levels == 0){
        m_levelsNb = std::min(5, static_cast<int>(std::log2f((float)std::max(m_width, m_height))));
    }else{
        m_levelsNb = levels;
    }

    if(nbChannels == 3){
        m_format = GL_RGB;
        m_internalFormat = GL_RGB16F;
    }else if(nbChannels == 4){
        m_format = GL_RGBA;
        m_internalFormat = GL_RGBA16F;
    }else if(nbChannels == 1){
        m_format = GL_R;
        m_internalFormat = GL_R16F;
    }else if(nbChannels == 2){
        m_format = GL_RG;
        m_internalFormat = GL_RG16F;
    }else{
        Logger::error("[TBO::init_multisample_data_f16] Channels nb not managed.\n");
        return;
    }

    glTextureStorage2DMultisample(
        m_id,               // GLuint texture,
        samples,            // GLsizei samples,
        m_internalFormat,   // GLenum internalformat,
        m_width,            // GLsizei width,
        m_height,           // GLsizei height,
        GL_TRUE             // GLboolean fixedsamplelocations
    );
}

auto TBO::init_multisample_data_f32(GLsizei width, GLsizei height, GLsizei depth, int nbChannels, GLsizei samples, int levels) -> void{

    m_width          = width;
    m_height         = height;
    m_depth          = depth;
    m_type           = GL_FLOAT;
    if(levels == 0){
        m_levelsNb = std::min(5, static_cast<int>(std::log2f((float)std::max(m_width, m_height))));
    }else{
        m_levelsNb = levels;
    }

    if(nbChannels == 3){
        m_format = GL_RGB;
        m_internalFormat = GL_RGB32F;
    }else if(nbChannels == 4){
        m_format = GL_RGBA;
        m_internalFormat = GL_RGBA32F;
    }else if(nbChannels == 1){
        m_format = GL_R;
        m_internalFormat = GL_R32F;
    }else if(nbChannels == 2){
        m_format = GL_RG;
        m_internalFormat = GL_RG32F;
    }else{
        Logger::error("[TBO::init_multisample_data_f32] Channels nb not managed.\n");
        return;
    }

    glTextureStorage2DMultisample(
        m_id,               // GLuint texture,
        samples,            // GLsizei samples,
        m_internalFormat,   // GLenum internalformat,
        m_width,            // GLsizei width,
        m_height,           // GLsizei height,
        GL_TRUE             // GLboolean fixedsamplelocations
    );
}

auto TBO::update_data(GLubyte *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void{
    if((width > m_width) || (height > m_width) || (depth > m_depth)){
        return;
    }
    gl_texture_sub_image(data, 0, width, height, depth, xOffset, yOffset, zOffset);
}

auto TBO::update_data(GLuint *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void{
    if((width > m_width) || (height > m_width) || (depth > m_depth)){
        return;
    }
    gl_texture_sub_image(data, 0, width, height, depth, xOffset, yOffset, zOffset);
}

auto TBO::update_data(GLfloat *data, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void{
    if((width > m_width) || (height > m_width) || (depth > m_depth)){
        return;
    }
    gl_texture_sub_image(data, 0, width, height, depth, xOffset, yOffset, zOffset);
}

auto TBO::generate_mipmap() -> void{
    glGenerateTextureMipmap(m_id);
}

auto TBO::gl_texture_storage() -> void{

    if(m_mode == TextureMode::texture_2d){

        glTextureStorage2D (
            m_id,               // GLuint texture
            m_levelsNb,         // GLsizei levels
            m_internalFormat,   // GLenum internalformat
            m_width,            // GLsizei width
            m_height            // GLsizei height
        );

    }else if(m_mode ==  TextureMode::texture_3d){

        glTextureStorage3D (
            m_id,               // GLuint texture
            m_levelsNb,         // GLsizei levels
            m_internalFormat,   // GLenum internalformat
            m_width,            // GLsizei width
            m_height,           // GLsizei height
            m_depth             // GLsizei depth
        );

    }else if(m_mode == TextureMode::cubemap){

        glTextureStorage2D (
            m_id,               // GLuint texture
            m_levelsNb,         // GLsizei levels
            m_internalFormat,   // GLenum internalformat
            m_width,            // GLsizei width
            m_height            // GLsizei height
        );

    }else{
        Logger::error("[TBO::gl_texture_storage] Texture mode not managed.\n");
    }
}


auto TBO::gl_texture_sub_image(void *data, GLint level, GLsizei width, GLsizei height, GLsizei depth, GLint xOffset, GLint yOffset, GLint zOffset) -> void{

    if(m_mode == TextureMode::texture_2d){

        // If a non-zero named buffer object is bound to the GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer)
        // while a texture image is specified, pixels is treated as a byte offset into the buffer object's data store.
        glTextureSubImage2D (
            m_id,               // GLuint texture
            level,              // GLint level
            xOffset,            // GLint xoffset
            yOffset,            // GLint yoffset
            width,              // GLsizei width
            height,             // GLsizei height
            m_format,           // GLenum format
            m_type,             // GLenum type
            data                // const void *pixels
        );

    }else if(m_mode ==  TextureMode::texture_3d){

        glTextureSubImage3D (
            m_id,               // GLuint texture
            level,              // GLint level
            xOffset,            // GLint xoffset
            yOffset,            // GLint yoffset
            zOffset,            // GLint zoffset
            width,              // GLsizei width
            height,             // GLsizei height
            depth,              // GLsizei depth
            m_format,           // GLenum format
            m_type,             // GLenum type
            data                // const void *pixels
        );

    }else if(m_mode == TextureMode::cubemap){

        glTextureSubImage3D (
            m_id,               // GLuint texture
            level,              // GLint level
            0,                  // GLint xoffset
            0,                  // GLint yoffset
            zOffset,            // GLint zoffset
            width,              // GLsizei width
            height,             // GLsizei height
            depth,              // GLsizei depth
            m_format,           // GLenum format
            m_type,             // GLenum type
            data                // const void *pixels
        );

    }else{
        Logger::error("[TBO::gl_texture_sub_image] Texture mode not managed.\n");
    }
}

auto TBO::get_hdr_texture_data(std::vector<GLfloat> &data) -> void{

    GLsizei size = m_width * m_height * m_depth * m_nbChannels;
    if(data.size() != static_cast<size_t>(size)){
        data.resize(size);
    }

    glGetTextureImage(
        id(),
        0,
        m_format,
        m_type,
        size,
        data.data()
        );

    // return;

    // // retrieve a sub-region of a texture image from a texture object
    // glGetTextureSubImage(
    //     id(),       // GLuint texture, Specifies the name of the source texture object.
    //                 //      Must be GL_TEXTURE_1D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY
    //                 //      or GL_TEXTURE_RECTANGLE. In specific, buffer and multisample textures are not permitted.
    //     0,          // GLint level: Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
    //     0,          // GLint xoffset: Specifies a texel offset in the x direction within the texture array.
    //     0,          // GLint yoffset: Specifies a texel offset in the y direction within the texture array.
    //     0,          // GLint zoffset: Specifies a texel offset in the z direction within the texture array.
    //     m_width,    // GLsizei width: Specifies the width of the texture subimage.
    //     m_height,   // GLsizei height: Specifies the height of the texture subimage.
    //     m_depth,    // GLsizei depth: Specifies the depth of the texture subimage.
    //     m_format,   // GLenum  format: Specifies the format of the pixel data.
    //                 //      The following symbolic values are accepted: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT and GL_STENCIL_INDEX.
    //     m_type,     // GLenum type: Specifies the data type of the pixel data. The following symbolic values are accepted:
    //                 //      GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2,
    //                 //      GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4,
    //                 //      GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
    //                 //      GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
    //     static_cast<GLsizei>(size), // GLsizei bufSize: Specifies the size of the buffer to receive the retrieved pixel data.
    //     data.data() // void *pixels:    Returns the texture subimage. Should be a pointer to an array of the type specified by type.
    //             );
}

auto TBO::debug_generate() -> void{
    generate();
}


