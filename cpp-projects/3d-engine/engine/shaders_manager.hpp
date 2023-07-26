
/*******************************************************************************
** Toolset-3d-engine                                                          **
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

// base
#include "utility/string_unordered_map.hpp"

// opengl-utility
#include "opengl/shader/shader.hpp"


namespace tool::graphics {

class ShadersManager{

    using Alias  = std::string;
    using AliasV = std::string_view;
    using Path   = std::string;
    using PathV  = std::string_view;

public:

    ~ShadersManager();

    static auto get_instance() -> ShadersManager*;

    auto load_shader(const Alias &alias, const std::vector<std::string> &paths) -> bool;
    auto reload_shader(AliasV alias) -> std::shared_ptr<gl::ShaderProgram>;
    auto reload_shader(gl::ShaderProgram *shader) -> std::shared_ptr<gl::ShaderProgram>;
    auto reload_all_shaders() -> void;
    auto add_shader(const Alias &alias, gl::ShaderProgram&& shader) -> void;

    auto get_shader(AliasV alias) const noexcept -> std::weak_ptr<gl::ShaderProgram>;
    auto get_ptr(AliasV alias) -> gl::ShaderProgram*;
    auto get_all_aliases() const noexcept -> std::vector<std::string_view>;

    auto unbind() -> void;

    auto debug() -> void;

private:

    s_umap<Alias, std::shared_ptr<gl::ShaderProgram>> shaders;    
};

}
