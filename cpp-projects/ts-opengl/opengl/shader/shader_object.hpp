
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

// glew
#include <GL/glew.h>

// base
#include "utility/tuple_array.hpp"
#include "utility/string_unordered_map.hpp"

namespace tool::gl {

using namespace std::literals::string_view_literals;

enum class ShaderType : std::uint8_t{
    vertex,fragment,geometry,tess_control,tess_eval,compute,
    SizeEnum
};

using Name  = std::string_view;
using Ext   = std::string_view;
using TShaderType = std::tuple<
    ShaderType,                                 GLenum,                     Name,             Ext>;
static constexpr TupleArray<ShaderType::SizeEnum, TShaderType> shaders = {{
    TShaderType{ShaderType::vertex,             GL_VERTEX_SHADER,           "vertex"sv,       ".vs"sv},
    TShaderType{ShaderType::fragment,           GL_FRAGMENT_SHADER,         "fragment"sv,     ".fs"sv},
    TShaderType{ShaderType::geometry,           GL_GEOMETRY_SHADER,         "geometry"sv,     ".gs"sv},
    TShaderType{ShaderType::tess_control,       GL_TESS_CONTROL_SHADER,     "tess_control"sv, ".tcs"sv},
    TShaderType{ShaderType::tess_eval,          GL_TESS_EVALUATION_SHADER,  "tess_eval"sv,    ".tes"sv},
    TShaderType{ShaderType::compute,            GL_COMPUTE_SHADER,          "compute"sv,      ".cs"sv},
}};

[[nodiscard]][[maybe_unused]] static constexpr auto get_gl_type(ShaderType t) -> GLenum{
    return shaders.at<0,1>(t);
}
[[nodiscard]][[maybe_unused]] static constexpr auto get_name(ShaderType t) -> std::string_view {
    return shaders.at<0,2>(t);
}
[[nodiscard]][[maybe_unused]] static constexpr auto get_ext(ShaderType t) -> std::string_view{
    return shaders.at<0,3>(t);
}

static inline s_umap<std::string, ShaderType> shadersTypeExtensions = {
    {".vs",         ShaderType::vertex},
    {".vert",       ShaderType::vertex},
    {"_vert.glsl",  ShaderType::vertex},
    {".vert.glsl",  ShaderType::vertex},
    {".gs",         ShaderType::geometry},
    {".geom",       ShaderType::geometry},
    {".geom.glsl",  ShaderType::geometry },
    {".tcs",        ShaderType::tess_control},
    {".tcs.glsl",   ShaderType::tess_control},
    {".tes",        ShaderType::tess_eval},
    {".tes.glsl",   ShaderType::tess_eval},
    {".fs",         ShaderType::fragment},
    {".frag",       ShaderType::fragment},
    {"_frag.glsl",  ShaderType::fragment},
    {".frag.glsl",  ShaderType::fragment},
    {".cs",         ShaderType::compute},
    {".cs.glsl",    ShaderType::compute}
};

}


