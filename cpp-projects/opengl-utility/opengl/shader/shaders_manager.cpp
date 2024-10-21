
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

#include "shaders_manager.hpp"

// base
#include "utility/logger.hpp"
#include "utility/string.hpp"

using namespace tool::graphics;
using namespace tool::gl;

struct Global{
    static inline std::unique_ptr<ShadersManager> instance = nullptr;
};

auto ShadersManager::get_instance() -> ShadersManager*{
    if(Global::instance == nullptr){
        Global::instance = std::make_unique<ShadersManager>();
    }
    return Global::instance.get();
}

ShadersManager::~ShadersManager(){
    for(auto &shader : shaders){
        shader.second->clean();
    }
}

auto ShadersManager::load_shader(const std::string &alias, std::span<const std::string> paths) -> bool{

    if(shaders.count(alias) != 0){
        Log::warning(std::format("[ShadersManager] Shader alias {} already exists, shader replaced.\n", alias));
    }

    auto shader = std::make_shared<gl::ShaderProgram>();
    if(!shader->load_from_files(paths)){
        Log::error(std::format("[ShadersManager] Cannot generate ShaderProgram from paths:\n{}", String::join(paths, "\n")));
        return false;
    }

    aliases.push_back(std::make_unique<std::string>(alias));
    aliasesV.push_back(*aliases.back());
    shaders[aliasesV.back()] = shader;
    return true;
}

auto ShadersManager::add_shader(const std::string &alias, ShaderProgram&& shader) -> void{

    if(shaders.count(alias) != 0){
        Log::warning(std::format("[ShadersManager] Shader alias {} already exists, shader replaced.\n", alias));
    }

    aliases.push_back(std::make_unique<std::string>(alias));
    aliasesV.push_back(*aliases.back());
    shaders[aliasesV.back()] = std::make_shared<ShaderProgram>(std::move(shader));
}

auto ShadersManager::reload_shader(std::string_view alias) -> std::shared_ptr<ShaderProgram>{

    if(shaders.contains(alias)){

        // retrieve current paths before cleaning shaders
        std::vector<std::string> shadersFilesPaths;
        for(const auto &path : shaders[alias]->shaders_file_paths()){
            shadersFilesPaths.push_back(path);
        }

        shaders[alias]->clean();
        shaders[alias] = std::make_shared<ShaderProgram>();
        Log::message(std::format("[ShadersM] Reload ShaderProgram with alias {}:\n", alias));
        if(!shaders[alias]->load_from_files(shadersFilesPaths)){
            Log::error(std::format("[ShadersM] Cannot reload ShaderProgram with alias {}:\n", alias));
            return nullptr;
        }
        return shaders[alias];
    }
    Log::error(std::format("[ShadersManager] Cannot find ShaderProgram with alias {}:\n", alias));
    return nullptr;
}

auto ShadersManager::reload_shader(ShaderProgram *shaderToBeReloaded) -> std::shared_ptr<ShaderProgram>{

    std::string alias;
    for(auto &shader : shaders){
        if(shader.second.get() == shaderToBeReloaded){
            return reload_shader(shader.first);
        }
    }
    Log::error("[ShadersManager] ShaderProgram pointer not found in manager.\n");
    return nullptr;
}

auto ShadersManager::reload_all_shaders() -> void{
    for(auto &shader : shaders){
        reload_shader(shader.first);
    }
}

auto ShadersManager::get_shader(std::string_view alias) const noexcept -> std::weak_ptr<ShaderProgram>{

    if(auto shader = shaders.find(alias);  shader != shaders.end()){
        return shader->second;
    }
    return {};
}

auto ShadersManager::get_ptr(std::string_view alias) -> ShaderProgram *{
    if(shaders.count(alias) != 0){
        return shaders[alias].get();
    }
    return nullptr;
}

auto ShadersManager::all_aliases() const noexcept -> std::span<const std::string_view>{
    return aliasesV;
}

auto ShadersManager::unbind() -> void{
    ShaderProgram::unbind();
}

auto ShadersManager::display_all() -> void{
    for(const auto &shader : shaders){
        Log::message(std::format("Shader [{}] [{}]\n", shader.first, shader.second->id()));
    }
}


