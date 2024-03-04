
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
#include <array>
#include <string>
#include <unordered_map>

// local
#include "opengl/gl_types.hpp"


namespace tool::gl {

struct UintData{
    const GLuint *v;
};

struct SizeData{
    const GLsizeiptr v;
};
struct AttriIndex{
    const GLuint v;
};
struct AttriSize{
    const GLint v;
};
struct AttriType{
    const GLenum v;
};
struct Stride{
    const GLsizei v;
};
struct RelativeOffset{
    const GLuint v;
};

struct AttribOffset{
    const GLvoid *v;
};


struct VerticesCount{
    GLsizei v;
};


static constexpr std::array<std::tuple<GlType, GLenum, const char*>,static_cast<size_t>(GlType::SizeEnum)> glTypeMapping ={{

    {GlType::bool_t,    GL_BOOL,        "bool"},
    {GlType::bvec2_t,   GL_BOOL_VEC2,   "bvec2"},
    {GlType::bvec3_t,   GL_BOOL_VEC3,   "bvec3"},
    {GlType::bvec4_t,   GL_BOOL_VEC4,   "bvec4"},

    {GlType::int_t,     GL_INT,        "int"},
    {GlType::ivec2_t,   GL_INT_VEC2,   "ivec2"},
    {GlType::ivec3_t,   GL_INT_VEC3,   "ivec3"},
    {GlType::ivec4_t,   GL_INT_VEC4,   "ivec4"},

    {GlType::uint_t,    GL_UNSIGNED_INT,        "uint"},
    {GlType::uivec2_t,  GL_UNSIGNED_INT_VEC2,   "uivec2"},
    {GlType::uivec3_t,  GL_UNSIGNED_INT_VEC3,   "uivec3"},
    {GlType::uivec4_t,  GL_UNSIGNED_INT_VEC4,   "uivec4"},

    {GlType::float_t,   GL_FLOAT,        "float"},
    {GlType::fvec2_t,   GL_FLOAT_VEC2,   "fvec2"},
    {GlType::fvec3_t,   GL_FLOAT_VEC3,   "fvec3"},
    {GlType::fvec4_t,   GL_FLOAT_VEC4,   "fvec4"},

    {GlType::double_t,  GL_FLOAT,        "double"},
    {GlType::dvec2_t,   GL_FLOAT_VEC2,   "dvec2"},
    {GlType::dvec3_t,   GL_FLOAT_VEC3,   "dvec3"},
    {GlType::dvec4_t,   GL_FLOAT_VEC4,   "dvec4"},

    {GlType::fmat2x2_t, GL_FLOAT_MAT2,   "fmat2x2"},
    {GlType::fmat3x3_t, GL_FLOAT_MAT3,   "fmat3x3"},
    {GlType::fmat4x4_t, GL_FLOAT_MAT4,   "fmat4x4"},

    {GlType::dmat2x2_t, GL_DOUBLE_MAT2,   "dmat2x2"},
    {GlType::dmat3x3_t, GL_DOUBLE_MAT3,   "dmat3x3"},
    {GlType::dmat4x4_t, GL_DOUBLE_MAT4,   "dmat4x4"},

    {GlType::sampler2d_t,       GL_SAMPLER_2D,   "sampler 2D"},
    {GlType::sampler2dShadow_t, GL_SAMPLER_2D_SHADOW,   "sampler 2D shadow"},
}};

[[maybe_unused]] static constexpr auto get_type(GLenum t) -> GlType{
    for (auto& m : glTypeMapping) {
        if (t == std::get<1>(m)) {
            return std::get<0>(m);
        }
    }
    return GlType::unknow;
}

[[maybe_unused]] static constexpr auto get_name(GlType t) -> const char*{
    for (auto& m : glTypeMapping) {
        if (t == std::get<0>(m)) {
            return std::get<2>(m);
        }
    }
    return "?";
}

[[maybe_unused]] static constexpr auto get_name(GLenum t) -> const char*{
    for (auto& m : glTypeMapping) {
        if (t == std::get<1>(m)) {
            return std::get<2>(m);
        }
    }
    return "?";
}

// uniform
struct UniformLocation{
    GLint v;
};
struct UniformType{
    GlType v;
};

// uniform block
struct UniformBlockLocation{
    GLuint v;
};
struct UniformBlockSize{
    GLint v;
};

struct UniformBlockElementInfo{
    GLuint index;
    GLint offset;
};




// sampler
struct Sampler2D{
    int v;
};
struct Sampler2DShadow{
    int v;
};
//static std::vector<SBS> generate_buffers(size_t count){
//    std::vector<SBS> buffers(count);
//    glCreateBuffers(count, &buffers[0].m_id);
//    return buffers;
//}


}
