
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

// glew
#include <GL/glew.h>

// base
#include "utility/tuple_array.hpp"
#include "utility/unordered_map.hpp"

namespace tool::gl {

using namespace std::literals::string_view_literals;

class Shader{
public:

    enum class Type : std::uint8_t{
        vertex,fragment,geometry,tess_control,tess_eval,compute,
        SizeEnum
    };

    using Name = std::string_view;
    using Ext = std::string_view;
    using TShader = std::tuple<
        Type,                         GLenum,                      Name,             Ext>;
    static constexpr TupleArray<Type::SizeEnum, TShader> shaders = {{
        TShader{Type::vertex,          GL_VERTEX_SHADER,           "vertex"sv,       ".vs"sv},
        TShader{Type::fragment,        GL_FRAGMENT_SHADER,         "fragment"sv,     ".fs"sv},
        TShader{Type::geometry,        GL_GEOMETRY_SHADER,         "geometry"sv,     ".gs"sv},
        TShader{Type::tess_control,    GL_TESS_CONTROL_SHADER,     "tess_control"sv, ".tcs"sv},
        TShader{Type::tess_eval,       GL_TESS_EVALUATION_SHADER,  "tess_eval"sv,    ".tes"sv},
        TShader{Type::compute,         GL_COMPUTE_SHADER,          "compute"sv,      ".cs"sv},
    }};

    static constexpr auto get_gl_type(Type t) -> GLenum{
        return shaders.at<0,1>(t);
    }
    static constexpr auto get_name(Type t) -> std::string_view {
        return shaders.at<0,2>(t);
    }
    static constexpr auto get_ext(Type t) -> std::string_view{
        return shaders.at<0,3>(t);
    }

    static inline umap<std::string, Shader::Type> extensions = {
        {".vs",         Shader::Type::vertex},
        {".vert",       Shader::Type::vertex},
        {"_vert.glsl",  Shader::Type::vertex},
        {".vert.glsl",  Shader::Type::vertex},
        {".gs",         Shader::Type::geometry},
        {".geom",       Shader::Type::geometry},
        {".geom.glsl",  Shader::Type::geometry },
        {".tcs",        Shader::Type::tess_control},
        {".tcs.glsl",   Shader::Type::tess_control},
        {".tes",        Shader::Type::tess_eval},
        {".tes.glsl",   Shader::Type::tess_eval},
        {".fs",         Shader::Type::fragment},
        {".frag",       Shader::Type::fragment},
        {"_frag.glsl",  Shader::Type::fragment},
        {".frag.glsl",  Shader::Type::fragment},
        {".cs",         Shader::Type::compute},
        {".cs.glsl",    Shader::Type::compute}
    };
};
}


