
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

// base
#include "utility/string_unordered_map.hpp"

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


    static auto get_instance() -> TexturesManager*;

    auto load_textures_from_directory(const std::string &directoryPath, const std::vector<TextureLoadingInfo> &infos) -> bool;
    auto load_cube_map(const std::string &basePath, const std::array<std::string,6> &extensions, const std::string &alias, bool flip = true) -> bool;

    auto get_texture(const std::string &textureAlias) -> std::weak_ptr<Texture2D>;
    auto get_texture_ptr(const std::string &textureAlias) -> Texture2D*;
    auto get_texture_info(const std::string &textureAlias, TextureOptions options = {}) -> TextureInfo;

    auto generate_texture2d_tbo(const std::string &tboAlias, const std::string &textureAlias, TextureOptions options = {}) -> bool;
    auto generate_projected_texture2d_tbo(const std::string &tboAlias, const std::string &textureAlias) -> bool;
    auto generate_cubemap_tbo(const std::string &tboAlias, const std::string &cubemapAlias) -> bool;

    auto texture_tbo(const std::string &tboAlias) -> gl::TBO*;
    auto texture_id(const std::string &tboAlias) -> GLuint;

    auto cube_map_tbo(const std::string &tboAlias) -> gl::TBO*;
    auto cube_map_id(const std::string &tboAlias) -> GLuint;

private:

    s_umap<std::string,  std::string> aliasPerPath;

    s_umap<std::string, std::shared_ptr<Texture2D>> textures2DPerAlias;
    s_umap<std::string, std::shared_ptr<CubeMap>> cubeMapsPerAlias;

    s_umap<std::string, std::unique_ptr<gl::Texture2D>> textures2DTboPerAlias;
    s_umap<std::string, std::unique_ptr<gl::CubeMap>> cubeMapsTboPerAlias;
};
}
