

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

#include "voxel_grid.hpp"

// local
#include "utility/logger.hpp"

#include <format>
#include <iostream>

using namespace tool::geo;

VoxelGrid::VoxelGrid(float voxelSize, Pt3f minBound, Pt3f maxBound) : voxelSize(voxelSize), origin(minBound), size(maxBound){
}

auto VoxelGrid::add_cloud(const ColoredCloudData &cloud, const Mat4f &model) -> void{

    bool hasColors = cloud.has_colors();
    for (size_t ii = 0; ii < cloud.size(); ++ii) {

        const auto &v =  cloud.vertices[ii];
        Pt4f v4{v.x(),v.y(),v.z(),1};
        auto vertex = (v4 * model).xyz();

        if(vertex > origin && vertex < size){

            Pt3f refCoord = (vertex - origin) / voxelSize;
            Pt3<int> voxelIndex = {
                static_cast<int>(std::floor(refCoord(0))),
                static_cast<int>(std::floor(refCoord(1))),
                static_cast<int>(std::floor(refCoord(2)))
            };

            if (hasColors) {
                voxelindexToAccPoint[voxelIndex].add(voxelIndex, cloud.colors[ii]);
            } else {
                voxelindexToAccPoint[voxelIndex].add(voxelIndex);
            }
        }
    }

}

auto VoxelGrid::compute_grid() -> void{

    if(voxelindexToAccPoint.empty()){
        return;
    }

    grid.clear();
    grid.reserve(voxelindexToAccPoint.size());

    for (const auto &accpoint : voxelindexToAccPoint) {
        grid[accpoint.second.voxel.index] = Voxel{
            accpoint.second.voxel.index,
            accpoint.second.get_average_color()
        };
    }

    voxelindexToAccPoint.clear();
}

auto VoxelGrid::convert_to_cloud(ColoredCloudData &cloud) -> void{

    cloud.resize(grid.size());

    size_t id = 0;
    for(const auto &voxel : grid){
        cloud.vertices[id] = origin + voxel.second.index.conv<float>() * voxelSize;        
        cloud.colors[id]   = voxel.second.color;        
        id++;        
    }
}

auto VoxelGrid::create_from_point_cloud_within_bounds(const ColoredCloudData &cloud, float voxelSize, Pt3f minBound, Pt3f maxBound) -> VoxelGrid{

    VoxelGrid voxelGrid(voxelSize, minBound, maxBound);
    if(voxelSize < 0.f){
        tool::Logger::error("[VoxelGrid::create_from_point_cloud_within_bounds] Voxel size <= 0.\n");
        return voxelGrid;
    }
    voxelGrid.add_cloud(cloud, Mat4f::identity());
    voxelGrid.compute_grid();
    return voxelGrid;
}
