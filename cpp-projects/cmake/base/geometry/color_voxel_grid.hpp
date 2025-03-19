
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
#include "voxel.hpp"
#include "color_cloud.hpp"
#include "utility/unordered_map.hpp"

namespace tool::geo{

struct ColorVoxelGrid{

    struct Pt3IntHashRepresentation {
        using is_avalanching = void;
        [[nodiscard]] auto operator()(const tool::geo::Pt3<int> &f) const noexcept -> uint64_t {
            static_assert(std::has_unique_object_representations_v<tool::geo::Pt3<int>>);
            return ankerl::unordered_dense::detail::wyhash::hash(&f, sizeof(f));
        }
    };

    ColorVoxelGrid(float voxelSize, tool::geo::Pt3f origin, tool::geo::Pt3f size);
    auto add_cloud(const ColorCloud &cloud, const Mat4f &model) -> void;
    auto compute_grid() -> void;

    auto convert_to_cloud(ColorCloud &cloud) -> void;
    auto copy_to_buffer(Buffer<CVoxel> &bBuffer) -> void;
    auto copy_to_buffer(Buffer<std::uint32_t> &xyzrgb) -> void;
    auto copy_to_buffer(Buffer<std::byte> &xyzrgb) -> void;

    static auto create_from_point_cloud_within_bounds(const ColorCloud &cloud, float voxelSize, tool::geo::Pt3f origin, tool::geo::Pt3f size) -> ColorVoxelGrid;

    float voxelSize = 0.1f;
    Pt3f origin = {};
    Pt3f size = {};
    umap<Pt3<int>, Voxel, Pt3IntHashRepresentation> grid;

private:

    umap<Pt3<int>, AvgColorVoxel, Pt3IntHashRepresentation> voxelindexToAccPoint;
    Buffer<std::tuple<bool,Pt3<int>>> buffer;
    std::vector<size_t> ids;
};

struct ColorVoxelGrid2{

    struct Pt3UintHashRepresentation {
        using is_avalanching = void;
        [[nodiscard]] auto operator()(const tool::geo::Pt3<std::uint32_t> &f) const noexcept -> uint64_t {
            static_assert(std::has_unique_object_representations_v<tool::geo::Pt3<std::uint32_t>>);
            return ankerl::unordered_dense::detail::wyhash::hash(&f, sizeof(f));
        }
    };

    ColorVoxelGrid2(float voxelSize, tool::geo::Pt3f origin, tool::geo::Pt3f size);
    auto add_cloud(const ColorCloud &cloud, const Mat4f &model) -> void;

    auto mix() -> void;
    auto copy_to_cloud(ColorCloud &cloud) -> void;
    auto copy_to_cvoxels(Buffer<CVoxel> &voxels) -> void;
    auto copy_to_cvoxels(std::span<CVoxel> voxels) -> void;
    auto copy_to_cvoxels(std::span<std::byte> voxelsBytes) -> void;

    auto compute_grid() -> void;
    auto test() -> void;

    float voxelSize = 0.1f;
    Pt3f origin = {};
    Pt3f size = {};
    Buffer<std::tuple<bool, Pt3<std::uint32_t>, Pt3f>> vertices;
    std::vector<size_t> ids;

private:

    // umap<Pt3<std::uint32_t>, UVoxel, Pt3UintHashRepresentation> grid;

    umap<Pt3<std::uint32_t>, size_t, Pt3UintHashRepresentation> gridId;
    // umap<std::uint32_t, size_t> xGridI;
    // umap<std::uint32_t, size_t> yGridI;
    // umap<std::uint32_t, size_t> ZGridI;
};

}
