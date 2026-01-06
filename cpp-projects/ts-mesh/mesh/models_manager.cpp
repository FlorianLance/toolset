
/*******************************************************************************
** Toolset-ts-mesh                                                            **
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

#include "models_manager.hpp"

// std
#include <future>
#include <set>
#include <format>

// base
#include "utility/logger.hpp"

// local
#include "assimp_loader.hpp"

using namespace tool;
using namespace tool::mesh;

struct Global{
    static inline std::unique_ptr<ModelsManager> instance = nullptr;
};

auto ModelsManager::get_instance() -> ModelsManager*{
    if(Global::instance == nullptr){
        Global::instance = std::make_unique<ModelsManager>();
    }
    return Global::instance.get();
}


auto ModelsManager::add(std::vector<std::pair<Alias, Path>> &&aliasPaths, bool forceReload) -> bool{

    // preprocess aliases/paths
    std::vector<std::pair<Alias, Path>> newV;
    newV.reserve(aliasPaths.size());

    std::set<Alias> aliases;
    std::set<Path> paths;
    for(auto &ap : aliasPaths){

        if(aliases.count(ap.first) != 0){ // check for duplicates aliases
            continue;
        }
        if(paths.count(ap.second) != 0){ // check for duplicates paths
            continue;
        }
        (aliases).insert(ap.first);
        aliases.insert(ap.second);

        if(!forceReload){
            if(aliasPerPath.count(ap.second) != 0){
                Log::error(std::format("[ModelsManager] Model with alias {} and path {} already loaded with alias {}, loading canceled.\n",
                        ap.first, ap.second, aliasPerPath[ap.second]));
                continue;
            }
        }

        newV.emplace_back(std::move(ap));
    }

    // do asynch loading
    s_umap<Alias, std::future<std::shared_ptr<ModelMesh>>> m;
    for(const auto &ap : newV){
        aliasPerPath[ap.first] = ap.second;
        m[ap.first] = std::async(std::launch::async, AiLoader::load_model, ap.second, false);
    }

    // add results to map
    for(auto &futureModel : m){
        models.add_element(futureModel.first, futureModel.second.get());
    }

    return true;
}

auto ModelsManager::add(const std::string &alias, const std::string &path) -> bool{

    if(aliasPerPath.count(path) != 0){
        Log::error(std::format(
            "[ModelsManager] Model with alias {} and path {} already loaded with alias {}, loading canceled.\n",
            alias, path, aliasPerPath[path])
        );
        return false;
    }

    if(models.has_alias(alias)){
        Log::error(std::format(
            "[ModelsManager] Model alias {} already used, it will be replaced by newly loaded model.\n",
            alias)
        );

    }
    if(auto model = AiLoader::load_model(path); model != nullptr){
        models.add_element(alias, std::move(model));
        aliasPerPath[path] = alias;
        return true;
    }

    return false;

}

auto ModelsManager::get(AliasV alias) const -> std::weak_ptr<ModelMesh> {
    if(auto model = models.get_element(alias); model){
        return model;
    }
    return {};
}

auto ModelsManager::get_animations_count(AliasV alias) const -> size_t{
    if(auto model = get_ptr(alias); model != nullptr){
        return model->animations.size();
    }
    return 0;
}

auto ModelsManager::get_animation(AliasV alias, size_t id) const -> Animation{
    if(auto model = get_ptr(alias); model != nullptr){
        if(id < model->animations.size()){
            return model->animations[id];
        }
    }
    return {};
}

auto ModelsManager::get_animation_name(AliasV alias, size_t id) const -> std::string_view{
    if(auto model = get_ptr(alias); model != nullptr){
        if(id < model->animations.size()){
            return model->animations[id].name;
        }
    }
    return "";
}


auto ModelsManager::get_ptr(AliasV alias) const -> ModelMesh*{
    return models.get_element_ptr(alias);
}



