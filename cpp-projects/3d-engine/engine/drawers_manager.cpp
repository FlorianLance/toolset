
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




