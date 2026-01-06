

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

#include "geometry_texture_2d_tbo.hpp"

using namespace tool::gl;


auto GeometryTexture2D::init_position(GLsizei w, GLsizei h) -> void{
    
    TBO::initialize();
    TBO::init_data_f32(w, h, 1, 3);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.wrapS = TextureWrapMode::clamp_to_edge; // test
    options.wrapT = TextureWrapMode::clamp_to_edge; // test
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}

auto GeometryTexture2D::init_color(GLsizei w, GLsizei h) -> void{
    
    TBO::initialize();
    TBO::init_data_u8(w, h, 1, 3);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}

auto GeometryTexture2D::init_ao(GLsizei w, GLsizei h) -> void{
    
    TBO::initialize();
    TBO::init_data_f32(w, h, 1, 1);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}


auto GeometryMultisampleTexture2D::init_color(GLsizei w, GLsizei h, GLsizei samples) -> void{
    
    TBO::initialize();
    TBO::init_multisample_data_u8(w, h, 1, 3, samples);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);

}

auto GeometryMultisampleTexture2D::init_position(GLsizei w, GLsizei h, GLsizei samples) -> void{
    
    TBO::initialize();
    TBO::init_multisample_data_f32(w, h, 1, 3, samples);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}

