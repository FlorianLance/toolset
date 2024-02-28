

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

#include "cube_map_tbo.hpp"

using namespace tool::gl;


auto CubeMap::load_textures(std::array<tool::graphics::Texture2D, 6> *textures) -> void{

    TBO::generate();

    const auto size  = (*textures)[0].size();
    if((*textures)[0].is_hdr()){
        init_data_f32(size.x(), size.y(), 1,  (*textures)[0].nb_channels());
    }else{
        init_data_u8(size.x(), size.y(), 1,  (*textures)[0].nb_channels());
    }

    for(GLint face = 0; face < 6; ++face){

        update_data(
            (*textures)[face].data(),
            m_width,
            m_height,
            m_depth,
            0,
            0,
            face
        );

        // glTextureSubImage3D(
        //     m_id,                       // GLuint texture
        //     0,                          // GLint level
        //     0,                          // GLint xoffset
        //     0,                          // GLint yoffset
        //     face,                       // GLint zoffset,
        //     m_width,                    // GLsizei width
        //     m_height,                   // GLsizei height
        //     m_depth,                    // GLsizei depth,
        //     m_format,                   // GLenum format
        //     m_type,                     // GLenum type
        //     (*textures)[face].data()    // const void *pixels
        // );
    }

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::linear;
    options.wrapS     = TextureWrapMode::clamp_to_edge;
    options.wrapT     = TextureWrapMode::clamp_to_edge;
    options.wrapR     = TextureWrapMode::clamp_to_edge;
    TBO::set_texture_options(options);

    TBO::generate_mipmap();
}
