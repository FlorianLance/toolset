
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


using namespace tool::graphics;


auto DCDeviceDrawer::update() -> void{

    m_locker.lock();
    if(m_lastFrame != nullptr){
        update_from_frame(0, m_lastFrame);
        m_redrawClouds = true;
        m_lastFrame = nullptr;
    }
    m_locker.unlock();

    if(m_redrawClouds || has_to_redraw_clouds()){
        draw_clouds_to_fbo();
    }
    m_redrawClouds = false;
}

auto DCDeviceDrawer::update_frame(std::shared_ptr<cam::DCFrame> frame) -> void{
    m_locker.lock();
    m_lastFrame = std::move(frame);
    m_locker.unlock();
}

auto DCDeviceDrawer::draw(bool focusWindow) -> void{
    draw_cloud_drawer_tab(0, focusWindow, "device"sv);
}

auto DCDeviceDrawer::save_current_cloud(const std::string &path) -> void{

    Logger::log("DCDeviceDrawer::save_cloud\n");

    m_locker.lock();
    auto frame = cloudsD.front()->lastFrame;
    m_locker.unlock();

    if(frame != nullptr){
        Logger::message(std::format("save_cloud: {}\n", path));

        if(auto cloud = frame->volume_buffer<geo::ColorCloud>(cam::DCVolumeBufferType::ColoredCloud)){
            tool::io::CloudIO::save_cloud(path, *cloud);

            for(size_t ii = 0; ii < cloud->size(); ++ii){
                cloud->colors[ii] = cloud->normals[ii];
            }
            auto nPath = path;
            tool::String::replace_first(nPath, ".obj", "_normals_colors.obj");
            tool::io::CloudIO::save_cloud(nPath, *cloud);
        }
    }
}
