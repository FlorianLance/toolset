
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
#include <cstdint>

// glew
#include <GL/glew.h>


namespace tool::gl {

enum class GlType : std::uint8_t{
    bool_t =0, bvec2_t, bvec3_t, bvec4_t,
    int_t, ivec2_t, ivec3_t, ivec4_t,
    uint_t, uivec2_t, uivec3_t, uivec4_t,
    float_t, fvec2_t, fvec3_t, fvec4_t,
    double_t, dvec2_t, dvec3_t, dvec4_t,
    fmat2x3_t, fmat2x4_t, fmat3x2_t, fmat3x4_t, fmat4x2_t, fma4x3_t, fmat2x2_t, fmat3x3_t, fmat4x4_t,
    dmat2x3_t, dmat2x4_t, dmat3x2_t, dmat3x4_t, dmat4x2_t, dma4x3_t, dmat2x2_t, dmat3x3_t, dmat4x4_t,
    sampler1d_t, sampler2d_t, sampler3d_t,
    samplerCube_t,
    sampler1dShadow_t, sampler2dShadow_t,
    sampler1dArray_t, sampler2dArray_t,
    sampler1dArrayShadow_t, sampler2dArrayShadow_t,
    sampler2dMS_t, sampler2dMSArray_t,samplerCubeShadow_t,
    samplerBuffer_t, sampler2dRect_t, sampler2dRectShadow_t,
    isampler1d_t, isampler2d_t, isampler3d_t, isamplerCube_t,
    isampler1dArray_t, isampler2dArray_t,
    isampler2dMS_t, isampler2dMSArray_t,
    isamplerBuffer_t, isampler2DRect_t,
    usampler1d_t, usampler2d_t, usampler3d_t,
    usamplerCube_t, usampler1dArray_t, usampler2dArray_t,
    usampler2dMS_t, usampler2dMSArray_t, usamplerBuffer,
    unknow, SizeEnum
};

}
