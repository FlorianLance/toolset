

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
#include "utility/time_guard.hpp"
// #include "io/cloud_io.hpp"

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

auto ColorVoxelGrid::copy_to_buffer(Buffer<CVoxel> &bBuffer) -> void{

    bBuffer.resize(grid.size());
    auto vs = bBuffer.span();

    size_t id = 0;
    size_t canceledPoints = 0;
    for(const auto &voxel : grid){

        auto col = (voxel.second.color*255.f).conv<std::uint8_t>();
        auto ids = voxel.second.index;

        if(ids.x() > 16383 || ids.y() > 4095 || ids.z() > 16383){
            ++canceledPoints;
            continue;
        }

        vs[id].red    = col.x();
        vs[id].green  = col.y();
        vs[id].blue   = col.z();

        vs[id].xIndex = ids.x();
        vs[id].yIndex = ids.y();
        vs[id].zIndex = ids.z();

        ++id;
    }

    if(canceledPoints != 0){
        Log::warning(std::format("[ColorVoxelGrid::copy_to_buffer] [{}] points were cancelled for being out of bounds.\n", canceledPoints));
    }
}


auto ColorVoxelGrid::copy_to_buffer(Buffer<uint32_t> &xyzrgb) -> void{


    // xyzrgb.resize(grid.size()*2);
    // xyzrgb.resize(grid.size()*3);
    xyzrgb.resize(grid.size()*4);
    // xyzrgb.resize(grid.size()*6);

    size_t id = 0;
    for(const auto &voxel : grid){

        // 0.84
        // auto ids = voxel.second.index.conv<std::uint32_t>();
        // auto col  = (voxel.second.color*255.f).conv<std::uint32_t>();
        // auto xy16  = ids.x() << 16 | ids.y();
        // auto zrg16 = ids.z() << 16 | col.x() << 8 | col.y();
        // xyzrgb[grid.size()*0 + id] = xy16;
        // xyzrgb[grid.size()*1 + id] = zrg16;

        // 0.24
        // auto col  = (voxel.second.color*255.f).conv<std::uint32_t>();
        // xyzrgb[grid.size()*0 + id] = col.x();
        // xyzrgb[grid.size()*1 + id] = col.y();
        // xyzrgb[grid.size()*2 + id] = col.z();

        // // 0.32
        // auto ids = voxel.second.index.conv<std::uint32_t>();
        // xyzrgb[grid.size()*0 + id] = ids.x();
        // xyzrgb[grid.size()*1 + id] = ids.y();
        // xyzrgb[grid.size()*2 + id] = ids.z();

        // 0.24
        // auto ids = voxel.second.index.conv<std::uint32_t>();
        // xyzrgb[grid.size()*0 + id] = ids.x();
        // xyzrgb[grid.size()*1 + id] = ids.y();
        // xyzrgb[grid.size()*2 + id] = ids.z();
        // xyzrgb[grid.size()*3 + id] = 0;

        // 0.43
        auto ids = voxel.second.index.conv<std::uint32_t>();
        auto col  = (voxel.second.color*255.f).conv<std::uint32_t>();
        auto pcol = col.x() << 16 | col.y() << 8 | col.z();
        xyzrgb[grid.size()*0 + id] = ids.x();
        xyzrgb[grid.size()*1 + id] = ids.y();
        xyzrgb[grid.size()*2 + id] = ids.z();
        xyzrgb[grid.size()*3 + id] = pcol;

        // // 0.51
        // auto ids = voxel.second.index.conv<std::uint32_t>();
        // auto col  = (voxel.second.color*255.f).conv<std::uint32_t>();
        // // auto pcol = col.x() << 16 | col.y() << 8 | col.z();
        // xyzrgb[id*6 + 0] = ids.x();
        // xyzrgb[id*6 + 1] = ids.y();
        // xyzrgb[id*6 + 2] = ids.z();
        // xyzrgb[id*6 + 3] = col.x();
        // xyzrgb[id*6 + 4] = col.y();
        // xyzrgb[id*6 + 5] = col.z();

        // auto ids = voxel.second.index.conv<std::uint32_t>();
        // auto col  = (voxel.second.color*255.f).conv<std::uint32_t>();
        // xyzrgb[grid.size()*3 + id] = col.x();
        // xyzrgb[grid.size()*4 + id] = col.y();
        // xyzrgb[grid.size()*5 + id] = col.z();

        ++id;
    }


}

auto ColorVoxelGrid::copy_to_buffer(Buffer<std::byte> &xyzrgb) -> void{
    xyzrgb.resize(grid.size()*16);
    auto data = std::span<std::uint32_t>(reinterpret_cast<std::uint32_t*>(xyzrgb.get_byte_data()), grid.size()*4);

    size_t id = 0;
    for(const auto &voxel : grid){
        auto ids = voxel.second.index.conv<std::uint32_t>();
        auto col  = (voxel.second.color*255.f).conv<std::uint32_t>();
        auto pcol = col.x() << 16 | col.y() << 8 | col.z();
        data[grid.size()*0 + id] = ids.x();
        data[grid.size()*1 + id] = ids.y();
        data[grid.size()*2 + id] = ids.z();
        data[grid.size()*3 + id] = pcol;
        ++id;
    }
}

auto ColorVoxelGrid::create_from_point_cloud_within_bounds(const ColorCloud &cloud, float voxelSize, Pt3f origin, Pt3f size) -> ColorVoxelGrid{

    ColorVoxelGrid voxelGrid(voxelSize, origin, size);
    if(voxelSize < 0.f){
        tool::Log::error("[VoxelGrid::create_from_point_cloud_within_bounds] Voxel size <= 0.\n");
        return voxelGrid;
    }
    voxelGrid.add_cloud(cloud, Mat4f::identity());
    voxelGrid.compute_grid();
    return voxelGrid;
}

ColorVoxelGrid2::ColorVoxelGrid2(float voxelSize, Pt3f origin, Pt3f size) : voxelSize(voxelSize), origin(origin), size(size){
}

auto ColorVoxelGrid2::add_cloud(const ColorCloud &cloud, const Mat4f &model) -> void{

    // TimeLogGuardMicroS tlg("ColorVoxelGrid2::add_cloud");

    size_t currentSize = vertices.size();
    vertices.resize(vertices.size() + cloud.size());

    // resize ids
    if(ids.size() < cloud.size()){
        ids.resize(cloud.size());
        std::iota(ids.begin(), ids.end(), 0);
    }

    bool hasColors = cloud.has_colors();
    geo::Pt3f defaultColor(1.f,1.f,1.f);

    std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + cloud.size(), [&](size_t id){

        const auto &vertex = cloud.vertices[id];
        const auto &color  = hasColors ? cloud.colors[id] : defaultColor;

        Pt4f v4{vertex.x(),vertex.y(),vertex.z(),1};
        Pt3f trVertex = (v4 * model).xyz();

        if(trVertex > origin && trVertex < size){
            Pt3f refCoord = (trVertex - origin) / voxelSize;
            vertices[currentSize + id] = {
                true,
                {
                    static_cast<std::uint32_t>(std::floor(refCoord(0))),
                    static_cast<std::uint32_t>(std::floor(refCoord(1))),
                    static_cast<std::uint32_t>(std::floor(refCoord(2)))
                },
                color
            };
        }else{
            vertices[currentSize + id] = {false,{},{}};
        }
    });
}

auto ColorVoxelGrid2::mix() -> void{

    TimeLogGuardMicroS tlg("ColorVoxelGrid2::mix");
    {
        // TimeLogGuardMicroS tlg("Mix::Sort");
        std::sort(std::execution::par_unseq, vertices.begin(), vertices.end(), [&](const auto &v1, const auto &v2){
            return std::get<1>(v1) < std::get<1>(v2);
        });
    }
    {
        // TimeLogGuardMicroS tlg("Mix::Erase");
        // auto erased =
        std::erase_if(vertices.values, [](const auto &v) {
            return !std::get<0>(v);
        });
        // Log::fmessage("-> {} {}\n", erased, vertices.values.size());
    }


    {
        // TimeLogGuardMicroS tlg("Mix::Remove duplicated points and mix their colors");

        size_t idCP = 0;
        // auto currentSize = vertices.size();
        Pt3<std::uint32_t> prevPosId;
        geo::Pt3f averageCol;
        float countC = 0.f;

        for(const auto &vertex : vertices){

            const auto &posId = std::get<1>(vertex);
            averageCol       += std::get<2>(vertex);
            countC           += 1.f;

            if(prevPosId != posId){
                vertices[idCP++] = {true, posId, averageCol/countC};
                averageCol       = {};
                countC           = 0.f;
            }
            prevPosId = posId;
        }

        vertices.resize(idCP);
        // Log::fmessage("New points {}, removed {} \n", vertices.size(), currentSize-vertices.size());
    }
}

auto ColorVoxelGrid2::copy_to_cloud(ColorCloud &cloud) -> void{

    TimeLogGuardMicroS tlg("ColorVoxelGrid2::convert_to_cloud");
    cloud.resize(vertices.size());

    // resize ids
    if(ids.size() < cloud.size()){
        ids.resize(cloud.size());
        std::iota(ids.begin(), ids.end(), 0);
    }

    std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + cloud.size(), [&](size_t id){
        const auto &vertex = vertices[id];
        cloud.vertices[id] = origin + std::get<1>(vertex).conv<float>() * voxelSize;
        cloud.colors[id]   = std::get<2>(vertex);
    });
}

auto ColorVoxelGrid2::copy_to_cvoxels(Buffer<CVoxel> &voxels) -> void{
    voxels.resize(vertices.size());
    copy_to_cvoxels(voxels.span());
}

auto ColorVoxelGrid2::copy_to_cvoxels(std::span<std::byte> voxelsBytes) -> void{
    copy_to_cvoxels(std::span<CVoxel>(reinterpret_cast<CVoxel*>(voxelsBytes.data()), voxelsBytes.size()/8));
}

auto ColorVoxelGrid2::compute_grid() -> void{

    // grid.clear();
    // grid.reserve(vertices.size());

    // for (const auto &vertex : vertices) {
    //     grid[std::get<1>(vertex)]= UVoxel{
    //         std::get<1>(vertex),
    //         std::get<2>(vertex)
    //     };
    // }

    gridId.clear();
    gridId.reserve(vertices.size());
    for(size_t idV = 0; idV < vertices.size(); ++idV){
        const auto &v = std::get<1>(vertices[idV]);
        gridId[v] = idV;
    }
}

auto ColorVoxelGrid2::test() -> void{

    Buffer<std::tuple<bool, Pt3<std::uint32_t>, Pt3f>> nv;
    nv.reserve(vertices.size());
    for(auto &vertex : vertices){
        const auto &ids = std::get<1>(vertex);
        if((ids.x() % 4 == 0) || (ids.y() % 4 == 0) || (ids.z() % 4 == 0)){
            nv.push_back(vertex);
        }
    }
    std::swap(nv,vertices);
}

auto ColorVoxelGrid2::copy_to_cvoxels(std::span<CVoxel> voxels) -> void{

    if(voxels.size() < vertices.size()){
        Log::ferror("[ColorVoxelGrid2::copy_to_cvoxels] Invalid span size, available [{}], vertices [{}].\n", voxels.size(), vertices.size());
        return;
    }

    size_t id = 0;
    size_t canceledPoints = 0;
    for(const auto &vertex : vertices){

        const auto &ids = std::get<1>(vertex);
        auto col = (std::get<2>(vertex)*255.f).conv<std::uint8_t>();

        if(ids.x() > CVoxel::maxX || ids.y() > CVoxel::maxY || ids.z() > CVoxel::maxY){
            ++canceledPoints;
            continue;
        }

        auto &voxel  = voxels[id];
        voxel.red    = col.x();
        voxel.green  = col.y();
        voxel.blue   = col.z();
        voxel.xIndex = ids.x();
        voxel.yIndex = ids.y();
        voxel.zIndex = ids.z();

        ++id;
    }

    if(canceledPoints != 0){
        Log::fwarning("[ColorVoxelGrid2::copy_to_cvoxels] [{}] points were cancelled for being out of bounds.\n", canceledPoints);
    }
}

