

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


#include "texture_2d_tbo.hpp"

using namespace tool::gl;


auto Texture2D::load_texture(tool::graphics::Texture2D *texture, TextureOptions options, int levels, GLint xOffset, GLint yOffset, GLint zOffset) -> void{

    TBO::generate();

    if(texture->is_hdr()){
        TBO::init_data_f32(texture->width(), texture->height(), 1, texture->nb_channels(), levels);
        TBO::update_data(reinterpret_cast<GLfloat*>(texture->data()), texture->width(), texture->height(), 1, xOffset, yOffset, zOffset);
    }else{
        TBO::init_data_u8(texture->width(), texture->height(), 1, texture->nb_channels(), levels);
        TBO::update_data(texture->data(), texture->width(), texture->height(), 1, xOffset, yOffset, zOffset);
    }

    TBO::set_texture_options(options);
    TBO::generate_mipmap();
}

auto Texture2D::load_projected_texture(tool::graphics::Texture2D *texture, int levels, GLint xOffset, GLint yOffset, GLint zOffset) -> void{

    TextureOptions options;
    options.wrapS       = TextureWrapMode::clamp_to_border;
    options.wrapT       = TextureWrapMode::clamp_to_border;
    options.minFilter   = TextureMinFilter::linear;
    options.magFilter   = TextureMagFilter::linear;
    load_texture(texture, options, levels, xOffset, yOffset, zOffset);
}

auto Texture2D::init_render(GLsizei w, GLsizei h, int nbChannels) -> void{
    TBO::generate();
    TBO::init_data_u8(w, h, 1, nbChannels);
}

auto Texture2D::init_hdr_render(GLsizei w, GLsizei h, int nbChannels) -> void{
    TBO::generate();
    TBO::init_data_f32(w, h, 1, nbChannels);
}

auto Texture2D::init_image_8u(GLsizei w, GLsizei h, int nbChannels) -> void{
    TBO::generate();
    TBO::init_data_u8(w, h, 1, nbChannels);
}

auto Texture2D::init_image_32u(GLsizei w, GLsizei h, int nbChannels) -> void{
    TBO::generate();
    TBO::init_data_u32(w, h, 1, nbChannels);
}

auto Texture2D::init_image_32f(GLsizei w, GLsizei h, int nbChannels) -> void{
    TBO::generate();
    TBO::init_data_f32(w, h, 1, nbChannels);
}

auto Texture2D::update_image_8u(GLubyte *data, GLsizei w, GLsizei h, GLint xOffset, GLint yOffset) -> void{
    TBO::update_data(data, w, h, 1, xOffset, yOffset, 0);
    TBO::generate_mipmap();
}

auto Texture2D::update_image_32u(GLuint *data, GLsizei w, GLsizei h, GLint xOffset, GLint yOffset) -> void{
    TBO::update_data(data, w, h, 1, xOffset, yOffset, 0);
    TBO::generate_mipmap();
}

auto Texture2D::update_image_32f(GLfloat *data, GLsizei w, GLsizei h, GLint xOffset, GLint yOffset) -> void{
    TBO::update_data(data, w, h, 1, xOffset, yOffset, 0);
    TBO::generate_mipmap();
}

auto Texture2D::init_or_update_8ui(GLsizei w, GLsizei h, int numChannels, uint8_t *data) -> void{

    if(id() == 0){
        init_image_8u(
            static_cast<GLsizei>(w),
            static_cast<GLsizei>(h),
            numChannels
        );
    }else if(
        (width()  != static_cast<GLsizei>(w)) ||
        (height() != static_cast<GLsizei>(h))){

        clean();
        init_image_8u(
            static_cast<GLsizei>(w),
            static_cast<GLsizei>(h),
            numChannels
        );
    }

    update_image_8u(
        reinterpret_cast<GLubyte*>(data),
        static_cast<GLsizei>(w),
        static_cast<GLsizei>(h), 0, 0
    );
}

auto Texture2D::load_data(float *data, GLsizei w, GLsizei h, int nbChannels, bool useInternal16Bits, TextureOptions options, int levels, GLint xOffset, GLint yOffset, GLint zOffset) -> void{

    TBO::generate();

    if(useInternal16Bits){
        TBO::init_data_f16(w, h, 1, nbChannels, levels);
    }else{
        TBO::init_data_f32(w, h, 1, nbChannels, levels);
    }
    TBO::update_data(data, w, h, 1, xOffset, yOffset, zOffset);
    TBO::set_texture_options(options);
    TBO::generate_mipmap();
}



