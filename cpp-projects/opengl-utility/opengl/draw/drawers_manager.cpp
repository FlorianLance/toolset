
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

#include "drawers_manager.hpp"

// std
#include <format>

// base
#include "utility/logger.hpp"


using namespace tool;
using namespace tool::graphics;


struct Global{
    static inline std::unique_ptr<DrawersManager> instance = nullptr;
};

auto DrawersManager::get_instance() -> DrawersManager*{
    if(Global::instance == nullptr){
        Global::instance = std::make_unique<DrawersManager>();
    }
    return Global::instance.get();
}

auto DrawersManager::add_drawer(const Alias &alias, std::shared_ptr<gl::Drawer> drawer, float scaleHint) -> void{

    if(drawer == nullptr){
        Logger::error(std::format("[DrawersManager] Drawer with alias: {} is null.\n",alias));
        return;
    }

    if(drawers.has_alias(alias)){
        Logger::error(std::format("[DrawersManager] Drawer alias already used: {}.\n",alias));
        return;
    }

    drawer->scaleHint = scaleHint;
    drawers.add_element(alias, std::move(drawer));
}

auto DrawersManager::get_drawer(AliasV alias) const noexcept -> std::weak_ptr<tool::gl::Drawer>{
    if(auto drawer = drawers.get_element(alias); drawer != nullptr){
        return drawer;
    }
    return {};
}

auto DrawersManager::get_alias(Id id) const noexcept -> DrawersManager::AliasV{
    if(drawers.has_id(id)){
        return drawers.get_alias(id).value();
    }
    Logger::error(std::format("[DrawersManager] No drawer with id {}.\n",id));
    return ""sv;
}

auto DrawersManager::get_id(DrawersManager::AliasV alias) const -> size_t{
    if(drawers.has_alias(alias)){
        return drawers.get_id(alias).value();
    }
    Logger::error(std::format("[DrawersManager] Drawer with alias {} not found.\n",alias));
    return 0;
}

auto DrawersManager::draw(AliasV alias, gl::ShaderProgram *shader) -> void{
    if(auto drawer = drawers.get_element_ptr(alias); drawer){
        drawer->draw(shader);
    }
}

auto DrawersManager::sub(std::vector<std::string_view> aliases) const -> IdAliasMapSharedPtr<gl::Drawer>{
    IdAliasMapSharedPtr<gl::Drawer> subDrawers;
    for(std::string_view alias : aliases){
        if(auto drawer = get_drawer(alias).lock(); drawer != nullptr){
            subDrawers.add_element(alias, std::move(drawer));
        }
    }
    return subDrawers;
}

auto DrawersManager::debug() -> void {
    drawers.debug();
//    for(size_t ii = 0; ii < drawers.count(); ++ii){
//        Logger::message(std::format("drawer {}:{}\n", ii, drawers.has_id(ii)));
//    }

//    for(size_t ii = 0; ii < drawers.count(); ++ii){
//        Logger::message(std::format("drawer {}:{}\n", ii, drawers.get_alias(ii).value()));
//    }
}




