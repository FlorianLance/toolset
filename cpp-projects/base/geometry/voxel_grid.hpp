
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

// local
#include "geometry/voxel.hpp"
#include "geometry/cloud.hpp"
#include "utility/unordered_map.hpp"

namespace tool::geo{

struct VoxelGrid{

    struct Pt3IntHashRepresentation {
        using is_avalanching = void;
        [[nodiscard]] auto operator()(const tool::geo::Pt3<int> &f) const noexcept -> uint64_t {
            static_assert(std::has_unique_object_representations_v<tool::geo::Pt3<int>>);
            return ankerl::unordered_dense::detail::wyhash::hash(&f, sizeof(f));
        }
    };

    VoxelGrid(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound);
    auto add_cloud(const ColoredCloudData &cloud, const Mat4f &model) -> void;
    auto compute_grid() -> void;

    auto convert_to_cloud(ColoredCloudData &cloud) -> void;

    static auto create_from_point_cloud_within_bounds(const ColoredCloudData &cloud, float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> VoxelGrid;

    float voxelSize = 0.1f;
    Pt3f origin = {};
    Pt3f size = {};
    umap<Pt3<int>, Voxel, Pt3IntHashRepresentation> grid;

private:

    umap<Pt3<int>, AvgColorVoxel, Pt3IntHashRepresentation> voxelindexToAccPoint;
};
}
