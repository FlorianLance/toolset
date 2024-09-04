
/*******************************************************************************
** Toolset-opengl-utility                                                     **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

// base
#include "geometry/cloud.hpp"

// local
#include "base_drawer.hpp"

namespace tool::gl {

class CloudPointsDrawer : public BaseDrawer{
public:
    CloudPointsDrawer() : BaseDrawer(DrawerType::Points){}
    auto initialize(bool dynamic, const geo::ColoredCloudData &cloud) -> void;
    auto initialize(bool dynamic, std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void;
    auto update(const geo::ColoredCloudData &cloud) -> void;
    auto update(std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void;
};


// VoxelsDrawer::VoxelsDrawer(std::span<const geo::Pt3<int>> voxels, std::span<const geo::Pt3f> colors){
//     init(voxels, colors);
// }

// auto VoxelsDrawer::init(std::span<const geo::Pt3<int>> voxels, std::span<const geo::Pt3f> colors) -> void{
//     clean();
//     if(!voxels.empty()){
//         drawableObject = std::make_unique<gl::Voxels>(voxels, colors);
//     }else{
//         drawableObject = nullptr;
//     }
// }

}
