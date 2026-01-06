
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

// assimp
#include <assimp/matrix4x4.h>

// base
#include "geometry/matrix4.hpp"

namespace tool::mesh {

static constexpr int nbBonesPerVertex = 4;

struct BoneData{

    std::array<int,nbBonesPerVertex> idS = {};
    std::array<float, nbBonesPerVertex> weights = {};

    auto add_bone_data(int boneID, float weight) -> bool;
};

struct BoneInfo{
    //    geo::Mat4f offset = geo::Mat4f::identity();
    aiMatrix4x4t<float> offset;
    geo::Mat4f final = geo::Mat4f::identity();
};


struct BonesHierarchy{
    std::string boneName;
    //    geo::Mat4f tr;
    aiMatrix4x4t<float> tr;
    std::vector<BonesHierarchy> children;
};

}
