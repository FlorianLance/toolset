
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
#include <memory>
#include <fstream>
#include <iostream>
#include <map>

// local
#include "geometry/point2.hpp"
#include "geometry/point3.hpp"
#include "geometry/point4.hpp"
#include "geometry/triangle3.hpp"

namespace tool::geo {


static constexpr int nbBonesPerVertex = 4;

struct BoneData{

    std::array<int,nbBonesPerVertex> idS = {};
    std::array<float, nbBonesPerVertex> weights = {};

    auto add_bone_data(int boneID, float weight) -> bool;
};

struct Mesh{

    std::vector<Pt3f> vertices;
    std::vector<Vec3f> normals;
    std::vector<TriIds> triIds;
    std::vector<Pt2f> tCoords;
    std::vector<Pt4f> colors;
    std::vector<Pt4f> tangents;
    std::vector<BoneData> bones;
    //    BVHNodeP<acc> accelerator = nullptr;

    Mesh() = default;
    Mesh(const Mesh& other) = default;
    Mesh& operator=(const Mesh& other) = default;
    Mesh(Mesh&& other) = default;
    Mesh& operator=(Mesh&& other) = default;

    constexpr auto triangle(size_t id) const -> Triangle3<float>{
        return {vertices[triIds[id].id1()],vertices[triIds[id].id2()],vertices[triIds[id].id3()]};
    }

    auto mean_position() const noexcept -> geo::Pt3f;
    auto generate_normals() noexcept -> void;
    auto generate_tangents() noexcept -> void;

    auto check() const -> void;

};
}

