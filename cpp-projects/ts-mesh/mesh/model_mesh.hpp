
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

#pragma once

// std
#include <map>

// base
#include "image/material.hpp"

// local
#include "bones.hpp"
#include "animation.hpp"
#include "mesh.hpp"

namespace tool::mesh {

struct SubModelMesh{
    std::string name;    
    Mesh mesh;
    img::Material *material = nullptr;
};

class ModelMesh{

public:

    // info
    std::string name;
    std::string directory = "";

    // meshes
    std::vector<std::shared_ptr<SubModelMesh>> meshes;

    // materials
    std::vector<img::Material> materials;

    // textures
    std::unordered_map<std::string, img::Texture2D> textures;

    // animations
    std::vector<Animation> animations;
    std::unordered_map<std::string_view, std::unordered_map<std::string,AnimationKeys>> animationsKeys;

    // bones
    std::vector<BoneInfo> bonesInfo;
    std::map<std::string, unsigned int> bonesMapping;
    BonesHierarchy bonesHierachy;

    aiMatrix4x4t<float> globalInverseTr;
    geo::Mat4f globalInverseTr2;

    // transform
    geo::Mat4f transform;

    // hierarchy
    ModelMesh *parent = nullptr;
    std::vector<std::shared_ptr<ModelMesh>> children;

public:

    ModelMesh() = default;
    ModelMesh(ModelMesh *parentModel) : parent(parentModel){}

    constexpr auto is_root() const noexcept -> bool{return parent == nullptr;}

    auto display_hierarchy() const -> void;
    auto get_all_tree_models() -> std::vector<ModelMesh*>;
    auto get_models_with_no_children(std::vector<ModelMesh*> &models) -> void;

    auto clean() -> void;
    auto get_bones_transform(BonesHierarchy *bh, std::string_view animationName, float time, const aiMatrix4x4t<float> &parentTr) -> void;
    auto update_animation(std::string_view animationName, float time) -> bool;
};
}

