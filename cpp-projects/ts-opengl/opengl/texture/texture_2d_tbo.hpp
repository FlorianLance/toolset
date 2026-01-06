
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

// local
#include "texture_buffer_object.hpp"

namespace tool::gl{

class Texture2D : public TBO{

public:

    Texture2D() : TBO(TextureMode::texture_2d){}

    auto load_texture(img::Texture2D *texture, TextureOptions options = {}, int levels = 0, GLint xOffset= 0, GLint yOffset = 0, GLint zOffset = 0) -> void;
    auto load_projected_texture(img::Texture2D *texture, int levels = 0, GLint xOffset= 0, GLint yOffset = 0, GLint zOffset = 0) -> void;
    auto load_data(float *data, GLsizei w, GLsizei h, int nbChannels, bool useInternal16Bits, TextureOptions options = {}, int levels = 0, GLint xOffset= 0, GLint yOffset = 0, GLint zOffset = 0) -> void;

    auto init_render(GLsizei w, GLsizei h, int nbChannels = 3) -> void;
    auto init_hdr_render(GLsizei w, GLsizei h, int nbChannels = 3) -> void;

    auto init_image_8u(GLsizei w, GLsizei h, int nbChannels) -> void;
    auto update_image_8u(GLubyte *data, GLsizei w, GLsizei h, GLint xOffset, GLint yOffset) -> void;

    auto init_image_32u(GLsizei w, GLsizei height, int nbChannels) -> void;
    auto update_image_32u(GLuint *data, GLsizei w, GLsizei h, GLint xOffset, GLint yOffset, bool generateMipMap) -> void;

    auto init_image_32f(GLsizei w, GLsizei h, int nbChannels) -> void;
    auto update_image_32f(GLfloat *data, GLsizei w, GLsizei h, GLint xOffset, GLint yOffset) -> void;

    auto init_or_update_8ui(GLsizei w, GLsizei h, int numChannels, uint8_t *data) -> void;

};





}


