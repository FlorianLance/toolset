
/*******************************************************************************
** Toolset-ts-base                                                            **
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

namespace tool::img{

struct TextureOptions{

    TextureType type = TextureType::diffuse;
    TextureOperation operation = TextureOperation::add;
    TextureMapping mapping = TextureMapping::UV;
    geo::Pt3<TextureMapMode> mapMode{
        TextureMapMode::wrap,
        TextureMapMode::wrap,
        TextureMapMode::wrap
    };

    int maxLevel = 1000;        // Sets the index of the highest defined mipmap level.

    TextureWrapMode wrapS       = TextureWrapMode::repeat;
    TextureWrapMode wrapT       = TextureWrapMode::repeat;
    TextureWrapMode wrapR       = TextureWrapMode::repeat;
    TextureMinFilter minFilter  = TextureMinFilter::nearest_mipmap_linear;
    TextureMagFilter magFilter  = TextureMagFilter::linear;
    geo::Pt4f borderColor{0.0f,0.0f,0.0f,0.0f};
    float minLOD = -1000.f;     // Sets the minimum level-of-detail parameter.
    float maxLOD = 1000.f;      // Sets the maximum level-of-detail parameter.
};
}
