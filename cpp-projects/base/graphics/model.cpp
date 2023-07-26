
/*******************************************************************************
** Toolset-base                                                               **
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


#include "model.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"

auto tool::graphics::Model::get_all_tree_models() -> std::vector<tool::graphics::Model *>{

    std::vector<Model*> models;
    models.push_back(this);

    for(const auto &child : children){
        auto childModels = child->get_all_tree_models();
        models.insert(std::end(models), std::begin(childModels), std::end(childModels));
    }

    return models;
}

auto tool::graphics::Model::get_models_with_no_children(std::vector<Model *> &models) -> void{

    if(children.size() == 0){
        models.push_back(this);
        return;
    }

    for(auto &child : children){
        child->get_models_with_no_children(models);
    }
}

auto tool::graphics::Model::display_hierarchy() const -> void{

    Logger::message(std::format("{} with name [{}] has [{}] meshes, [{}] animations and [{}] sub nodes.\n"sv,
        parent != nullptr ? "Node"sv : "Root node"sv,
        name,
        gmeshes.size(),
        animations.size(),
        children.size())
    );
    if(gmeshes.size() > 0){
        Logger::message("#Meshes:\n");
        size_t id = 0;
        for(const auto &gmesh : gmeshes){
            Logger::message(std::format("subMesh: [{}]\n", id++));
            const geo::Mesh *mesh = &gmesh->mesh;
            Logger::message(std::format("\tvertices: [{}]\n\tnormals: [{}]\n\ttriangles: [{}]\n\ttextures coords: [{}]\n\ttangents: [{}]\n\tbones: [{}]\n"sv,
                mesh->vertices.size(),
                mesh->normals.size(),
                mesh->triIds.size(),
                mesh->tCoords.size(),
                mesh->tangents.size(),
                mesh->bones.size()
            ));
        }
    }
    if(animations.size() > 0){
        Logger::message("#Animations:\n");
        size_t id = 0;
        for(const auto &animation : animations){
            Logger::message(std::format("\tanimation: [{}][{}]\n\tduration: [{}]\n\tticks per second: [{}]\n"sv,
                id++,
                animation.name,
                animation.duration,
                animation.ticksPerSecond
            ));
        }
    }

    for(const auto &child : children){
        child->display_hierarchy();
    }
}
