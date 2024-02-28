

/*******************************************************************************
** Toolset-base                                                               **
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
#include "geometry/point3.hpp"
#include "geometry/point4.hpp"
#include "texture_enum.hpp"

namespace tool::graphics{

struct TextureOptions{

    TextureType type = TextureType::diffuse;
    TextureOperation operation = TextureOperation::add;
    TextureMapping mapping = TextureMapping::UV;
    geo::Pt3<TextureMapMode> mapMode{
        TextureMapMode::wrap,
        TextureMapMode::wrap,
        TextureMapMode::wrap
    };

    // mixed
    TextureWrapMode wrapS = TextureWrapMode::repeat; // Sets the wrap parameter for texture coordinate s
    TextureWrapMode wrapT = TextureWrapMode::repeat; // Sets the wrap parameter for texture coordinate t
    TextureWrapMode wrapR = TextureWrapMode::repeat; // Sets the wrap parameter for texture coordinate r
    TextureMinFilter minFilter = TextureMinFilter::nearest_mipmap_linear;
    TextureMagFilter magFilter = TextureMagFilter::linear;

    geo::Pt4f borderColor{0.0f,0.0f,0.0f,0.0f};

    int maxLevel = 1000;    // Sets the index of the highest defined mipmap level.
                            // This is an integer value. The initial value is 1000.
    int minLOD = -1000;     // Sets the minimum level-of-detail parameter.
                            // This floating-point value limits the selection of highest resolution mipmap (lowest mipmap level). The initial value is -1000.
    int maxLOD = 1000;      // Sets the maximum level-of-detail parameter.
                            // This floating-point value limits the selection of the lowest resolution mipmap (highest mipmap level). The initial value is 1000.
};
}
