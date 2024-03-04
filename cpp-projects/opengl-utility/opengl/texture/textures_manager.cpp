
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

#include "textures_manager.hpp"

// base
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::graphics;

struct Global{
    static inline std::unique_ptr<TexturesManager> instance = nullptr;
};

auto TexturesManager::get_instance() -> TexturesManager*{
    if(Global::instance == nullptr){
        Global::instance = std::make_unique<TexturesManager>();
    }
    return Global::instance.get();
}

auto TexturesManager::load_textures_from_directory(const Path &directoryPath, std::vector<TextureLoadingInfo> infos) -> bool{

    for(const auto &info : infos){

        // check alias
        if(textures.count(info.alias) != 0){
            Logger::error(std::format("[TexturesManager::load_textures_from_directory] Texture alias already used: {}\n", info.alias));
            continue;
        }

        // check if texture has already been loaded
        const std::string textureFilePath = {directoryPath + "/" + info.fileName};
        if(aliasPerPath.count(textureFilePath) != 0){

            // find previously loaded texture
            textures[info.alias] = textures[aliasPerPath[textureFilePath]];
            Logger::error(std::format("[TexturesManager::load_textures_from_directory] Texture file {} has already been loaded with alias {}\n", textureFilePath, info.alias));
            continue;
        }

        // load texture
        auto texture = std::make_shared<graphics::Texture2D>();
        if(!texture->load_2d_image_file_data(textureFilePath, info.flip, info.targetNbChannels)){
            Logger::error(std::format("[TexturesManager::load_textures_from_directory] Cannot load texture file: {}\n", textureFilePath));
            return false;
        }

        // add to map
        texturesAliases.push_back(info.alias);
        aliasPerPath[textureFilePath] = info.alias;
        textures[info.alias] = texture;
    }

    return true;
}

auto TexturesManager::load_cube_map(const Path &basePath, const std::array<std::string, 6> &extensions, const Alias &alias, bool flip) -> bool{

    // check alias
    if(cubeMaps.count(alias) != 0){
        Logger::error(std::format("[TexturesManager::load_cube_map] CubeMap alias already used: {}\n", alias));
        return false;
    }

    // check if textures have already been loaded
    std::array<std::string, 6> texturesPath;
    for(size_t ii = 0; ii < extensions.size(); ++ii){
        texturesPath[ii] = {basePath + extensions[ii]};
        if(aliasPerPath.count(texturesPath[ii]) != 0){
            Logger::error(std::format("[TexturesManager::load_cube_map] Cubemap texture file already loaded: {}\n", texturesPath[ii]));
            continue;
        }
    }

    // load cube map
    auto cubeMap = std::make_shared<graphics::CubeMap>();
    if(!cubeMap->load_2d_images_files(texturesPath, flip)){
        Logger::error("[TexturesManager::load_cube_map] Cannot load cubemap files. \n");
        return false;
    }

    // add to map
    for(const auto &path : texturesPath){
        aliasPerPath[path] = alias;
    }
    cubeMapsAliases.push_back(alias);
    cubeMaps[alias] = cubeMap;

    return true;
}

auto TexturesManager::get_texture(const Alias &textureAlias) -> std::weak_ptr<Texture2D>{
    if(textures.count({textureAlias}) != 0){
        return textures[{textureAlias}];
    }
    return {};
}

auto TexturesManager::get_texture_ptr(const Alias &textureAlias) -> Texture2D*{
    if(textures.count({textureAlias}) != 0){
        return textures[{textureAlias}].get();
    }
    return nullptr;
}

TextureInfo TexturesManager::get_texture_info(const Alias &textureAlias, TextureOptions options){
    return TextureInfo{get_texture_ptr(textureAlias),options};
}

auto TexturesManager::generate_texture2d_tbo(const Alias &tboAlias, const Alias &textureAlias, TextureOptions options) -> bool{

    if(textures.count(textureAlias) == 0){
        Logger::error(std::format("[TexturesManager::generate_texture2d_tbo] Texture alias {} doesn't exist\n", textureAlias));
        return false;
    }

    if(texturesTBOs.count(tboAlias) != 0){
        Logger::error(std::format("[TexturesManager::generate_texture2d_tbo] TBO alias already used: {}\n", tboAlias));
        return false;
    }

    tool::gl::Texture2D tbo;
    tbo.load_texture(textures[textureAlias].get(), {options});
    tboAliases.push_back(tboAlias);
    texturesTBOs[tboAlias] = std::move(tbo);

    return true;
}

auto TexturesManager::generate_projected_texture2d_tbo(const Alias &tboAlias, const Alias &textureAlias) -> bool{

    if(textures.count(textureAlias) == 0){
        Logger::error(std::format("[TexturesManager::generate_projected_texture2d_tbo] Texture alias {} doesn't exist\n", textureAlias));
        return false;
    }

    if(texturesTBOs.count(tboAlias) != 0){
        Logger::error(std::format("[TexturesManager::generate_projected_texture2d_tbo] TBO alias already used: {}\n", tboAlias));
        return false;
    }

    tool::gl::Texture2D tbo;
    tbo.load_projected_texture(textures[textureAlias].get());
    tboAliases.push_back(tboAlias);
    texturesTBOs[tboAlias] = std::move(tbo);

    return true;
}

auto TexturesManager::generate_cubemap_tbo(const Alias &tboAlias, const Alias &cubemapAlias) -> bool{

    if(cubeMaps.count(cubemapAlias) == 0){
        Logger::error(std::format("[TexturesManager::generate_cubemap_tbo] Texture alias {} doesn't exist\n", cubemapAlias));
        return false;
    }

    if(cubeMapsTBOs.count(tboAlias) != 0){
        Logger::error(std::format("[TexturesManager::generate_cubemap_tbo] TBO alias already used: {}\n", tboAlias));
        return false;
    }

    tool::gl::CubeMap tbo;
    tbo.load_textures(&cubeMaps[cubemapAlias]->textures);
    tboAliases.push_back(tboAlias);
    cubeMapsTBOs[tboAlias] = std::move(tbo);

    return true;
}

auto TexturesManager::texture_id(const TexturesManager::Alias &tboAlias) -> GLuint{
    if(auto tbo = texture_tbo(tboAlias); tbo != nullptr){
        return tbo->id();
    }
    return 0;
}

auto TexturesManager::cube_map_tbo(const TexturesManager::Alias &tboAlias) -> gl::TBO*{
    if(cubeMapsTBOs.count(tboAlias) != 0){
        return &cubeMapsTBOs[tboAlias];
    }
    return nullptr;
}

auto TexturesManager::cube_map_id(const TexturesManager::Alias &tboAlias) -> GLuint{
    if(auto tbo = cube_map_tbo(tboAlias); tbo != nullptr){
        return tbo->id();
    }
    return 0;
}

auto TexturesManager::texture_tbo(const Alias &tboAlias) -> gl::TBO*{
    if(texturesTBOs.count(tboAlias) != 0){
        return &texturesTBOs[tboAlias];
    }
    return nullptr;
}

