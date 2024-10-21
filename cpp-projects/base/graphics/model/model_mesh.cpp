
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


#include "model_mesh.hpp"

// std
#include <format>

// assimp
#include "assimp/scene.h"

// local
#include "utility/logger.hpp"

auto tool::graphics::ModelMesh::get_all_tree_models() -> std::vector<tool::graphics::ModelMesh *>{
    
    std::vector<ModelMesh*> models;
    models.push_back(this);

    for(const auto &child : children){
        auto childModels = child->get_all_tree_models();
        models.insert(std::end(models), std::begin(childModels), std::end(childModels));
    }

    return models;
}

auto tool::graphics::ModelMesh::get_models_with_no_children(std::vector<ModelMesh *> &models) -> void{

    if(children.size() == 0){
        models.push_back(this);
        return;
    }

    for(auto &child : children){
        child->get_models_with_no_children(models);
    }
}

auto tool::graphics::ModelMesh::clean() -> void{

    std::vector<size_t> childrenToRemove;
    for(size_t ii = 0; ii < children.size(); ++ii){
        if(children[ii]->meshes.size() == 0 && children[ii]->children.size() == 0 && children[ii]->animations.size() == 0){
            childrenToRemove.push_back(ii);
        }
    }

    std::reverse(std::begin(childrenToRemove), std::end(childrenToRemove));
    for(const auto &id : childrenToRemove){
        children.erase(std::begin(children) + static_cast<int>(id));
    }
    
    if(meshes.size() == 0 && children.size() == 0 && animations.size() == 0){
        parent->clean();
    }
}

auto tool::graphics::ModelMesh::get_bones_transform(BonesHierarchy *bh, std::string_view animationName, float time, const aiMatrix4x4t<float> &parentTr) -> void{

    auto nodeTr = bh->tr;

    if(animationsKeys.count(animationName) != 0){

        if(animationsKeys[animationName].count(bh->boneName) != 0){

            AnimationKeys *keys = &animationsKeys[animationName][bh->boneName];

            // compute interpolation
            geo::Pt3f scaling   = keys->calculate_interpolated_scaling(time);
            geo::Quatf rotation = keys->calculate_interpolated_rotation(time);
            geo::Pt3f position  = keys->calculate_interpolated_position(time);

            aiVector3D Scaling;
            Scaling.x = scaling.x();
            Scaling.y = scaling.y();
            Scaling.z = scaling.z();

            aiQuaternion Rotation;
            Rotation.x = rotation.x;
            Rotation.y = rotation.y;
            Rotation.z = rotation.z;
            Rotation.w = rotation.w;

            aiVector3D Position;
            Position.x = position.x();
            Position.y = position.y();
            Position.z = position.z();

            nodeTr = aiMatrix4x4t(Scaling, Rotation, Position);


            //                aiVector3t<float> aiTr,aiRot,aiSc;
            //                q.Decompose(aiSc,aiRot,aiTr);
            //                auto r = geo::Pt3f{aiRot.x,aiRot.y,aiRot.z};
            //                nodeTr = geo::Mat4f::transform(
            //                    geo::Pt3f{aiSc.x,aiSc.y,aiSc.z},
            ////                    geo::Pt3f{rad_2_deg(r.x()),rad_2_deg(r.y()),rad_2_deg(r.z())},
            //                    geo::Pt3f{(r.x()),(r.y()),(r.z())},
            //                    geo::Pt3f{aiTr.x,aiTr.y,aiTr.z}
            //                );


            //                const auto &m = q;//.Transpose();
            //                nodeTr = geo::Mat4f
            //                {
            //                    m.a1,m.a2,m.a3,m.a4,
            //                    m.b1,m.b2,m.b3,m.b4,
            //                    m.c1,m.c2,m.c3,m.c4,
            //                    m.d1,m.d2,m.d3,m.d4,
            //                };


        }
    }

    auto globalTr = parentTr * nodeTr;
    //        geo::Mat4f globalTr = nodeTr*parentTr;
    if(animationsKeys.count(animationName) != 0){

        std::string bName = bh->boneName;
        //            std::cout << "----------> " << bName << " ";
        //            if(bonesMapping.count(bName) != 0){


        //            }
        //            auto pos = std::find(std::begin(bonesMapping));
        //            if (pos != std::string::npos) {
        //                bName.erase(pos, bName.size()-pos);
        //            }

        if(bonesMapping.count(bName) != 0){
            //                std::cout << "find bone " << bName << "\n";

            auto id = bonesMapping[bName];
            auto m = globalInverseTr* globalTr * bonesInfo[id].offset;

            //                bonesInfo[id].final =
            //                {
            //                    m.a1,m.a2,m.a3,m.a4,
            //                    m.b1,m.b2,m.b3,m.b4,
            //                    m.c1,m.c2,m.c3,m.c4,
            //                    m.d1,m.d2,m.d3,m.d4,
            //                };
            bonesInfo[id].final =
                {
                    m.a1,m.b1,m.c1,m.d1,
                    m.a2,m.b2,m.c2,m.d2,
                    m.a3,m.b3,m.c3,m.d3,
                    m.a4,m.b4,m.c4,m.d4,
                };

            //                std::cout << "final " << bonesInfo[id].final << "\n";

            //                bonesInfo[id].final = globalInverseTr.value()* globalTr * bonesInfo[id].offset;
            //                bonesInfo[id].final = globalInverseTr.value();//* bonesInfo[id].offset;

            //                bonesInfo[id].final =  bonesInfo[id].offset* globalTr* globalInverseTr.value();
            //                std::cout << "final: " << id << " " << bonesInfo[id].final << "\n";
        }else{
            //                std::cerr << "Cannot find bone " << bName << " in mapping: " << bonesMapping.size() << "\n";
        }
    }

    for(auto &b : bh->children){
        get_bones_transform(&b, animationName, time, globalTr);
    }
}

auto tool::graphics::ModelMesh::update_animation(std::string_view animationName, float time) -> bool{

    const Animation *animationP = nullptr;
    for(const auto &animation : animations){
        if(animation.name == animationName){
            animationP = &animation;
            break;
        }
    }
    if(animationP == nullptr){
        return false;
    }

    float TicksPerSecond = almost_equal(animationP->ticksPerSecond, 0.0) ? static_cast<float>(animationP->ticksPerSecond) : 25.f;
    float TimeInTicks    = time * TicksPerSecond;
    float AnimationTime  = fmod(TimeInTicks, static_cast<float>(animationP->duration));

    // retrieve bones transforms
    aiMatrix4x4t<float> m;
    get_bones_transform(&bonesHierachy, animationName, AnimationTime, m);
    return true;
}

auto tool::graphics::ModelMesh::display_hierarchy() const -> void{
    
    Log::message(std::format("{} with name [{}] has [{}] meshes, [{}] animations and [{}] sub nodes.\n"sv,
                                parent != nullptr ? "Node"sv : "Root node"sv,
                                name,
                                meshes.size(),
                                animations.size(),
                                children.size())
                    );
    if(meshes.size() > 0){
        Log::message("#Meshes:\n");
        size_t id = 0;
        for(const auto &gmesh : meshes){
            Log::message(std::format("subMesh: [{}]\n", id++));
            const Mesh *mesh = &gmesh->mesh;
            Log::message(std::format("\tvertices: [{}]\n\tnormals: [{}]\n\ttriangles: [{}]\n\ttextures coords: [{}]\n\ttangents: [{}]\n\tbones: [{}]\n"sv,
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
        Log::message("#Animations:\n");
        size_t id = 0;
        for(const auto &animation : animations){
            Log::message(std::format("\tanimation: [{}][{}]\n\tduration: [{}]\n\tticks per second: [{}]\n"sv,
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
