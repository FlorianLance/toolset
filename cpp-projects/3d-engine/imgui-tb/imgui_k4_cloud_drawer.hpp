


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
#include "camera/kinect4/k4_device_settings.hpp"
#include "camera/kinect4/k4_display_settings.hpp"


// opengl
#include "opengl/gl_texture.hpp"
#include "opengl/draw/drawer.hpp"

// local
#include "imgui_texture_ui_drawer.hpp"

namespace tool::graphics {

struct K4CloudDrawer{

    // info
    std::int32_t lastFrameId = -1;
    std::shared_ptr<camera::K4Frame> lastFrame = nullptr;

    // drawers
    gl::CloudPointsDrawer cpD;
    ImGuiTextureUiDrawer colorD;
    ImGuiTextureUiDrawer depthD;
    ImGuiTextureUiDrawer infraD;
    gl::SphereDrawer spD;

    // settings
    geo::Mat4f model = geo::Mat4f::identity();
    camera::K4CloudDisplaySettings display;

    // joints sub models
    size_t nbBodies = 0;
    std::vector<std::array<std::tuple<bool,geo::Mat4f>, camera::k4JointsCount>> jointsModels;

    // textures
    gl::Texture2D colorT;
    gl::Texture2D depthT;
    gl::Texture2D infraT;

    // init
    auto initialize() -> void;
    auto reset() -> void;
    // # from frame
    auto init_from_frame(std::shared_ptr<camera::K4Frame> frame) -> bool;
    // # from data
    auto init_from_colored_cloud_data(const geo::ColoredCloudData &cloudData) -> bool;
};
}
