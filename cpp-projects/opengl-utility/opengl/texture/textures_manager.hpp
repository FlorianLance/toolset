
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

// local
#include "texture_2d_tbo.hpp"
#include "cube_map_tbo.hpp"

namespace tool::graphics {

struct TextureLoadingInfo{
    std::string alias;
    std::string fileName;
    bool flip = true;
    int targetNbChannels = 4;
};

class TexturesManager{

public:

    using Alias = std::string;
    using Path  = std::string;

    static auto get_instance() -> TexturesManager*;

    auto load_textures_from_directory(const Path &directoryPath, std::vector<TextureLoadingInfo> infos) -> bool;
    auto load_cube_map(const Path &basePath, const std::array<std::string,6> &extensions, const Alias &alias, bool flip = true) -> bool;

    auto get_texture(const Alias &textureAlias) -> std::weak_ptr<Texture2D>;
    auto get_texture_ptr(const Alias &textureAlias) -> Texture2D*;
    auto get_texture_info(const Alias &textureAlias, TextureOptions options = {}) -> TextureInfo;

    auto generate_texture2d_tbo(const Alias &tboAlias, const Alias &textureAlias, TextureOptions options = {}) -> bool;
    auto generate_projected_texture2d_tbo(const Alias &tboAlias, const Alias &textureAlias) -> bool;
    auto generate_cubemap_tbo(const Alias &tboAlias, const Alias &cubemapAlias) -> bool;

    auto texture_tbo(const Alias &tboAlias) -> gl::TBO*;
    auto texture_id(const Alias &tboAlias) -> gl::TextureName;

    auto cube_map_tbo(const Alias &tboAlias) -> gl::TBO*;
    auto cube_map_id(const Alias &tboAlias) -> gl::TextureName;

//    constexpr size_t textures_count() const noexcept {return texturesAliases.size();}
//    constexpr size_t cube_maps_count() const noexcept {return cubeMapsAliases.size();}
//    constexpr size_t tbo_count() const noexcept {return tboAliases.size();}

private:

    std::vector<Alias> texturesAliases;
    std::vector<Alias> cubeMapsAliases;
    std::vector<Alias> tboAliases;

    std::unordered_map<Path,  Alias> aliasPerPath;

    std::unordered_map<Alias, std::shared_ptr<Texture2D>> textures;
    std::unordered_map<Alias, std::shared_ptr<CubeMap>> cubeMaps;
    std::unordered_map<Alias, gl::TBO> texturesTBOs;
    std::unordered_map<Alias, gl::TBO> cubeMapsTBOs;
};
}
