
/*******************************************************************************
** Toolset-3d-engine                                                          **
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

// base
#include "camera/kinect4/k4_frame.hpp"
#include "camera/kinect4/k4_filters.hpp"

// 3d-engine
#include "imgui_k4_clouds_scene_drawer.hpp"

namespace tool::graphics {

struct K4DeviceDrawer : public K4CloudsSceneDrawer{

    auto initialize() -> void;
    auto update() -> void;
    auto draw(bool focusWindow) -> void;

    auto update_filters(const camera::K4Filters &filters) -> void{
//        if(!plane1D){
//            plane1D = std::make_unique<gl::GridDrawer>();
//        }
//        plane1Tr = geo::look_at(filters.p1Pos, filters.p1Pos + geo::normalize(filters.p1Rot), geo::Vec3f(1,0,0));
//         = geo::transform(geo::Pt3f{1.f,1.f,1.f}, filters.p1Rot, filters.p1Pos);
    }

    auto update_frame(std::shared_ptr<camera::K4Frame> frame) -> void;

    auto save_cloud(const std::string &path) -> void;

private:
    std::mutex locker;
    bool redrawClouds = false;
    std::shared_ptr<camera::K4Frame> lastFrame = nullptr;
    std::int32_t previousFrameId = 0;
};
}
