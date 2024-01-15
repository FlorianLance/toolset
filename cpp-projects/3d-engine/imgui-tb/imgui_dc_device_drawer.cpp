
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

#include "imgui_dc_device_drawer.hpp"

// std
#include <format>

// base
#include "io/cloud_io.hpp"
#include "utility/string.hpp"
#include "utility/logger.hpp"

// local
// #define IMGUI_DEFINE_MATH_OPERATORS
// #include "imgui/imgui.h"

using namespace tool::graphics;

auto DCDeviceDrawer::initialize() -> void{
    DCCloudsSceneDrawer::initialize(1);
}

auto DCDeviceDrawer::update() -> void{

    locker.lock();
    if(lastFrame != nullptr){
        if(previousFrameId != lastFrame->idCapture){
            update_from_frame(0, lastFrame);
            previousFrameId = lastFrame->idCapture;
            redrawClouds = true;
        }
    }
    locker.unlock();

    if(redrawClouds || has_to_redraw_clouds()){
        draw_clouds_to_fbo();
    }
    redrawClouds = false;
}

auto DCDeviceDrawer::draw(bool focusWindow) -> void{
    draw_cloud_drawer_tab(0, focusWindow, "device"sv);
}

auto DCDeviceDrawer::update_frame(std::shared_ptr<camera::DCFrame> frame) -> void{
    locker.lock();
    lastFrame = frame;
    locker.unlock();
}

auto DCDeviceDrawer::save_cloud(const std::string &path) -> void{

    Logger::message("DCDeviceDrawer::save_cloud\n");

    std::shared_ptr<camera::DCFrame> frame = nullptr;
    locker.lock();
    frame = lastFrame;
    locker.unlock();

    if(frame != nullptr){
        Logger::message(std::format("save_cloud: {}\n", path));
        tool::io::CloudIO::save_cloud(path, frame->cloud);
        auto c = frame->cloud;
        for(size_t ii = 0; ii < c.size(); ++ii){
            c.vertices[ii] += c.normals[ii]*0.2f;
        }

        auto nPath = path;
        tool::String::replace_first(nPath, ".obj", "_.obj");
        tool::io::CloudIO::save_cloud(nPath, c);
    }
}
