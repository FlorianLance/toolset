
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

// base
#include "utility/id_alias_map.hpp"

// local
#include "base_drawer.hpp"

namespace tool::graphics {

struct Scale{
    float v;
};

class DrawersManager{

    using Alias  = std::string;
    using AliasV = std::string_view;
    using Id     = size_t;

public:

    static auto get_instance() -> DrawersManager*;

    // template <typename D, typename... A>
    // auto add_drawer(const Alias &alias, Scale scaleHint, A&&... args){
    //     add_drawer(alias, std::make_shared<D>(std::forward<A>(args)...), scaleHint.v);
    // }
    // auto add_drawer(const std::string &alias, std::shared_ptr<gl::Drawer> drawer, float scaleHint = 1.f) -> void;
    auto add_drawer(const std::string &alias, std::shared_ptr<gl::BaseDrawer> drawer, float scaleHint = 1.f) -> void;

    auto get_drawer(std::string_view alias) const noexcept -> std::weak_ptr<gl::BaseDrawer>;
    auto get_alias(Id id) const noexcept -> std::string_view;
    auto get_id(std::string_view alias) const -> size_t;
    inline auto count() const noexcept -> size_t{return drawers2.count();}

    auto draw(std::string_view alias, gl::ShaderProgram *shader = nullptr) -> void;

    auto sub(std::vector<std::string_view> aliases) const -> IdAliasMapSharedPtr<gl::BaseDrawer>;

    auto debug() -> void;
private:

    // IdAliasMapSharedPtr<gl::Drawer> drawers;
    IdAliasMapSharedPtr<gl::BaseDrawer> drawers2;
};
}



