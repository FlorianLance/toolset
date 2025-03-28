
/*******************************************************************************
** Toolset-imgui-opengl-engine                                                **
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
#include "imgui_dc_clouds_scene_drawer.hpp"

namespace tool::graphics {

struct DCDirectDrawer : public DCCloudsSceneDrawer{

    auto initialize(size_t nbGrabbers) -> void override;
    auto update() -> void;
    auto draw(bool focus) -> void;
    auto draw_only_clouds() -> void;
    
    auto update_frame(size_t idGrabber, std::shared_ptr<cam::DCFrame> frame) -> void;
    auto save_current_cloud(size_t idC, const std::string &path) -> void;


private:

    std::mutex m_locker;
    bool m_redrawClouds = false;
    std::vector<std::shared_ptr<cam::DCFrame>> m_lastFrames;
    std::vector<std::shared_ptr<cam::DCFrame>> m_swapLastFrames;

    // std::vector<std::unique_ptr<std::mutex>> m_lockers;

    bool m_allTabOpened = false;    
    size_t m_currentTabOpened = 0;
};
}
