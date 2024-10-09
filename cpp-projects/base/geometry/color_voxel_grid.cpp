

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

#include "color_voxel_grid.hpp"

// std
#include <execution>

// local
#include "geometry/point4.hpp"
#include "utility/logger.hpp"

using namespace tool::geo;

ColorVoxelGrid::ColorVoxelGrid(float voxelSize, Pt3f origin, Pt3f size) : voxelSize(voxelSize), origin(origin), size(size){
}

auto ColorVoxelGrid::add_cloud(const ColorCloud &cloud, const Mat4f &model) -> void{

    buffer.resize(cloud.size());

    ids.resize(cloud.size());
    std::iota(ids.begin(), ids.end(), 0);

    bool hasColors = cloud.has_colors();
    std::for_each(std::execution::par_unseq, std::begin(ids), std::end(ids), [&](size_t id){
        const auto &v =  cloud.vertices[id];
        Pt4f v4{v.x(),v.y(),v.z(),1};
        Pt3f vertex = (v4 * model).xyz();
        if(vertex > origin && vertex < size){
            Pt3f refCoord = (vertex - origin) / voxelSize;
            buffer[id] = {true,{
                static_cast<int>(std::floor(refCoord(0))),
                static_cast<int>(std::floor(refCoord(1))),
                static_cast<int>(std::floor(refCoord(2)))
            }};
        }else{
            buffer[id] = {false,{}};
        }
    });

    voxelindexToAccPoint.reserve(voxelindexToAccPoint.size() + buffer.size());
    std::for_each(std::execution::unseq, std::begin(ids), std::end(ids), [&](size_t id){
        if(std::get<0>(buffer[id])){
            auto &voxelIndex = std::get<1>(buffer[id]);
            if (hasColors) {
                voxelindexToAccPoint[voxelIndex].add(voxelIndex, cloud.colors[id]);
            } else {
                voxelindexToAccPoint[voxelIndex].add(voxelIndex);
            }
        }
    });


    // bool hasColors = cloud.has_colors();
    // for (size_t ii = 0; ii < cloud.size(); ++ii) {

    //     const auto &v =  cloud.vertices[ii];
    //     Pt4f v4{v.x(),v.y(),v.z(),1};
    //     auto vertex = (v4 * model).xyz();

    //     if(vertex > origin && vertex < size){

    //         Pt3f refCoord = (vertex - origin) / voxelSize;
    //         Pt3<int> voxelIndex = {
    //             static_cast<int>(std::floor(refCoord(0))),
    //             static_cast<int>(std::floor(refCoord(1))),
    //             static_cast<int>(std::floor(refCoord(2)))
    //         };

    //         if (hasColors) {
    //             voxelindexToAccPoint[voxelIndex].add(voxelIndex, cloud.colors[ii]);
    //         } else {
    //             voxelindexToAccPoint[voxelIndex].add(voxelIndex);
    //         }
    //     }
    // }

}

auto ColorVoxelGrid::compute_grid() -> void{

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

auto ColorVoxelGrid::convert_to_cloud(ColorCloud &cloud) -> void{

    cloud.resize(grid.size());

    size_t id = 0;
    for(const auto &voxel : grid){
        cloud.vertices[id] = origin + voxel.second.index.conv<float>() * voxelSize;        
        cloud.colors[id]   = voxel.second.color;        
        id++;        
    }
}

auto ColorVoxelGrid::create_from_point_cloud_within_bounds(const ColorCloud &cloud, float voxelSize, Pt3f origin, Pt3f size) -> ColorVoxelGrid{

    ColorVoxelGrid voxelGrid(voxelSize, origin, size);
    if(voxelSize < 0.f){
        tool::Logger::error("[VoxelGrid::create_from_point_cloud_within_bounds] Voxel size <= 0.\n");
        return voxelGrid;
    }
    voxelGrid.add_cloud(cloud, Mat4f::identity());
    voxelGrid.compute_grid();
    return voxelGrid;
}
