
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
#include <string_view>

// glew
#include <GL/glew.h>

// base
#include "utility/tuple_array.hpp"

namespace tool::gl {

using namespace std::literals::string_view_literals;

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
    unknow,
    SizeEnum
};

using TGlTypes = std::tuple<
    GlType,                     GLenum,                 std::string_view>;

static constexpr TupleArray<GlType::SizeEnum, TGlTypes> glTypes ={{
    TGlTypes
    {GlType::bool_t,            GL_BOOL,                "bool"sv},
    {GlType::bvec2_t,           GL_BOOL_VEC2,           "bvec2"sv},
    {GlType::bvec3_t,           GL_BOOL_VEC3,           "bvec3"sv},
    {GlType::bvec4_t,           GL_BOOL_VEC4,           "bvec4"sv},
    {GlType::int_t,             GL_INT,                 "int"sv},
    {GlType::ivec2_t,           GL_INT_VEC2,            "ivec2"sv},
    {GlType::ivec3_t,           GL_INT_VEC3,            "ivec3"sv},
    {GlType::ivec4_t,           GL_INT_VEC4,            "ivec4"sv},
    {GlType::uint_t,            GL_UNSIGNED_INT,        "uint"sv},
    {GlType::uivec2_t,          GL_UNSIGNED_INT_VEC2,   "uivec2"sv},
    {GlType::uivec3_t,          GL_UNSIGNED_INT_VEC3,   "uivec3"sv},
    {GlType::uivec4_t,          GL_UNSIGNED_INT_VEC4,   "uivec4"sv},
    {GlType::float_t,           GL_FLOAT,               "float"sv},
    {GlType::fvec2_t,           GL_FLOAT_VEC2,          "fvec2"sv},
    {GlType::fvec3_t,           GL_FLOAT_VEC3,          "fvec3"sv},
    {GlType::fvec4_t,           GL_FLOAT_VEC4,          "fvec4"sv},
    {GlType::double_t,          GL_FLOAT,               "double"sv},
    {GlType::dvec2_t,           GL_FLOAT_VEC2,          "dvec2"sv},
    {GlType::dvec3_t,           GL_FLOAT_VEC3,          "dvec3"sv},
    {GlType::dvec4_t,           GL_FLOAT_VEC4,          "dvec4"sv},
    {GlType::fmat2x2_t,         GL_FLOAT_MAT2,          "fmat2x2"sv},
    {GlType::fmat3x3_t,         GL_FLOAT_MAT3,          "fmat3x3"sv},
    {GlType::fmat4x4_t,         GL_FLOAT_MAT4,          "fmat4x4"sv},
    {GlType::dmat2x2_t,         GL_DOUBLE_MAT2,         "dmat2x2"sv},
    {GlType::dmat3x3_t,         GL_DOUBLE_MAT3,         "dmat3x3"sv},
    {GlType::dmat4x4_t,         GL_DOUBLE_MAT4,         "dmat4x4"sv},
    {GlType::sampler2d_t,       GL_SAMPLER_2D,          "sampler 2D"sv},
    {GlType::sampler2dShadow_t, GL_SAMPLER_2D_SHADOW,   "sampler 2D shadow"sv},

}};

[[nodiscard]][[maybe_unused]] static constexpr auto get_type(GLenum t) -> GlType{
    return glTypes.at<1,0>(t);
}

[[nodiscard]][[maybe_unused]] static constexpr auto get_name(GlType t) -> std::string_view{
    return glTypes.at<0,2>(t);
}

[[nodiscard]][[maybe_unused]] static constexpr auto get_name(GLenum t) -> std::string_view{
    return glTypes.at<1,2>(t);
}

}
