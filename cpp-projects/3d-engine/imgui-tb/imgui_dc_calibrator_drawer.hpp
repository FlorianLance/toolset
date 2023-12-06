
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
#include "camera/settings/dc_calibrator_settings.hpp"

// local
#include "imgui_dc_clouds_scene_drawer.hpp"

namespace tool::graphics {

struct DCCalibratorDrawer : public DCCloudsSceneDrawer{

    auto initialize(size_t nbGrabbers) -> void;
    auto set_data(int sourceId, int modelId,  std::vector<camera::DCCalibratorGrabberData>* grabbersData) -> void;
    auto draw() -> void;

    auto update_grabber_model(size_t idGrabber, const camera::DCModelSettings &model) -> void;
    auto update_grabber_cloud_display(size_t idGrabber, const camera::DCCloudDisplaySettings &cloudDisplay) -> void;
    auto update_settings(const DCCalibratorDrawerSettings &settings) -> void;

private:

    bool m_redrawClouds = false;
    DCCalibratorDrawerSettings m_settings;
};
}
