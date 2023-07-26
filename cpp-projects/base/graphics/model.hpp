

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

#pragma once

// std
#include <optional>

// base
#include "geometry/matrix4.hpp"
#include "geometry/quaternion.hpp"

// local
#include "geometry/mesh.hpp"
#include "texture.hpp"
#include "material.hpp"

// assimp
#include "assimp/scene.h"
#include "assimp/matrix4x4.h"

namespace tool::graphics {

template <typename acc>
struct GMesh{
    std::string name;
    Material *material = nullptr;
    geo::Mesh mesh;
};

struct BoneInfo{
//    geo::Mat4f offset = geo::Mat4f::identity();
    aiMatrix4x4t<float> offset;
    geo::Mat4f final = geo::Mat4f::identity();
};

struct AnimationKeys{

    std::vector<double> positionTimes;
    std::vector<geo::Pt3f> positionKeys;

    std::vector<double> rotationTimes;
    std::vector<geo::Quatf> rotationKeys;

    std::vector<double> scalingTimes;
    std::vector<geo::Pt3f> scalingKeys;


    auto find_position(float time) const noexcept -> size_t{
        for(size_t ii = 0; ii < positionTimes.size()-1; ++ii){
            if(time < static_cast<float>(positionTimes[ii+1])){
                return ii;
            }
        }
        return 0;
    }

    auto find_rotation(float time) const noexcept -> size_t{
        for(size_t ii = 0; ii < rotationTimes.size()-1; ++ii){
            if(time < static_cast<float>(rotationTimes[ii+1])){
                return ii;
            }
        }
        return 0;
    }

    auto find_scaling(float time) const noexcept -> size_t{
        for(size_t ii = 0; ii < scalingTimes.size()-1; ++ii){
            if(time < static_cast<float>(scalingTimes[ii+1])){
                return ii;
            }
        }
        return 0;
    }

    auto calculate_interpolated_scaling(float time) const noexcept -> geo::Pt3f{

        if(scalingKeys.empty()){
            return {};
        }

        if(scalingKeys.size() == 1){
            return scalingKeys[0];
        }

        size_t scalingId =find_scaling(time);
        size_t nextScalingIndex = scalingId +1;

        float deltaTime = static_cast<float>(scalingTimes[nextScalingIndex] - scalingTimes[scalingId]);
        float factor = (time - static_cast<float>(scalingTimes[scalingId]))/deltaTime;

        geo::Pt3f start = scalingKeys[scalingId];
        geo::Pt3f end   = scalingKeys[nextScalingIndex];
        geo::Pt3f delta = end - start;

        return start + delta * factor;
    }

    auto calculate_interpolated_position(float time) const noexcept -> geo::Pt3f{

        if(positionKeys.empty()){
            return {};
        }

        if(positionKeys.size() == 1){
            return positionKeys[0];
        }

        size_t positionId = find_position(time);
        size_t nextPositionIndex = positionId +1;

        float deltaTime = static_cast<float>(positionTimes[nextPositionIndex] - positionTimes[positionId]);
        float factor = (time - static_cast<float>(positionTimes[positionId]))/deltaTime;

        geo::Pt3f start = positionKeys[positionId];
        geo::Pt3f end   = positionKeys[nextPositionIndex];
        geo::Pt3f delta = end - start;

        return start + delta * factor;
    }

    auto calculate_interpolated_rotation( float time) -> geo::Quatf{

        if(rotationKeys.empty()){
            return {};
        }

        if(rotationKeys.size() == 1){
            return rotationKeys[0];
        }

        size_t rotationId = find_rotation(time);
        size_t nextRotationIndex = rotationId +1;

        float deltaTime = static_cast<float>(rotationTimes[nextRotationIndex] - rotationTimes[rotationId]);
        float factor = (time - static_cast<float>(rotationTimes[rotationId]))/deltaTime;

        geo::Quatf start = rotationKeys[rotationId];
        geo::Quatf end   = rotationKeys[nextRotationIndex];
        return normalize(slerp(start, end, factor));
    }
};

struct Animation{
    std::string name;
    double duration;
    double ticksPerSecond;
};



struct BonesHierarchy{
    std::string boneName;
//    geo::Mat4f tr;
    aiMatrix4x4t<float> tr;
    std::vector<BonesHierarchy> children;
};


class Model{

public:

    // info
    std::string name;
    std::string directory = "";

    // meshes
    std::vector<std::shared_ptr<GMesh<float>>> gmeshes;

    // materials
    std::vector<Material> materials;

    // textures
    std::unordered_map<std::string, Texture2D> textures;

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
    Model *parent = nullptr;
    std::vector<std::shared_ptr<Model>> children;

public:

    Model() = default;
    Model(Model *parentModel) : parent(parentModel){}

    constexpr auto is_root() const noexcept -> bool{return parent == nullptr;}

    auto display_hierarchy() const -> void;
    auto get_all_tree_models() -> std::vector<Model*>;
    auto get_models_with_no_children(std::vector<Model*> &models) -> void;

    void clean(){

        std::vector<size_t> childrenToRemove;
        for(size_t ii = 0; ii < children.size(); ++ii){
            if(children[ii]->gmeshes.size() == 0 && children[ii]->children.size() == 0 && children[ii]->animations.size() == 0){
                childrenToRemove.push_back(ii);
            }
        }

        std::reverse(std::begin(childrenToRemove), std::end(childrenToRemove));
        for(const auto &id : childrenToRemove){
            children.erase(std::begin(children) + static_cast<int>(id));
        }

        if(gmeshes.size() == 0 && children.size() == 0 && animations.size() == 0){
            parent->clean();
        }
    }







    void get_bones_transform(BonesHierarchy *bh, std::string_view animationName, float time, const aiMatrix4x4t<float> &parentTr){

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


    auto update_animation(std::string_view animationName, float time) -> bool{

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
};
}

