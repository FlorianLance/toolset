
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

#include "imgui_k4_calibrator_drawer.hpp"

using namespace tool::graphics;

auto K4CalibratorDrawer::initialize(size_t nbGrabbers) -> void{
    K4CloudsSceneDrawer::initialize(2*nbGrabbers);

    for(size_t ii = 0; ii < nbGrabbers; ++ii){
        auto &cdC = cloudsD[ii].display;
        cdC.forceCloudColor = true;
        auto &cdP = cloudsD[nbGrabbers  + ii].display;
        cdP.forceCloudColor = true;
    }
}

auto K4CalibratorDrawer::set_data(int sourceId, int modelId, std::vector<camera::DCCalibratorGrabberData>* grabbersData) -> void{

    auto allSelection = modelId == grabbersData->size();
    for(size_t ii = 0; ii < grabbersData->size(); ++ii){

        auto grabberData = &(*grabbersData)[ii];

        // calibration
        update_from_colored_cloud_data(grabberData->id, grabberData->calibrationCloud);

        // processsed
        update_from_colored_cloud_data(grabbersData->size() + grabberData->id, grabberData->processedCloud);

        auto visibilty = (ii == modelId) || (ii == sourceId) || allSelection;

        auto &cdC = cloudsD[ii].display;
        cdC.cloudVisible = m_settings.displayCalibrationCloud && visibilty;

        auto &cdP = cloudsD[grabbersData->size()  + ii].display;
        cdP.cloudVisible = m_settings.displayProcessedCloud && visibilty;
    }
    m_redrawClouds = true;
}

auto K4CalibratorDrawer::draw() -> void{

    if(m_redrawClouds || has_to_redraw_clouds()){
        draw_clouds_to_fbo();
    }
    m_redrawClouds = false;

    draw_all_clouds_drawers_in_one_tab(false, false, false, true, "Calibration clouds");
}

auto K4CalibratorDrawer::update_grabber_model(size_t idGrabber, const camera::DCModelSettings &model) -> void{

    auto tr = model.compute_full_transformation();
    cloudsD[idGrabber].model                      = tr;
    cloudsD[(cloudsD.size()/2) + idGrabber].model = tr;
    m_redrawClouds = true;
}

auto K4CalibratorDrawer::update_grabber_cloud_display(size_t idGrabber, const camera::DCCloudDisplaySettings &cloudDisplay) -> void{

    auto &cdC = cloudsD[idGrabber].display;
    cdC.forceCloudColor = true;
    cdC.cloudColor = cloudDisplay.cloudColor;
    cdC.sizePoints = cloudDisplay.sizePoints;
    cdC.useVoxels  = cloudDisplay.useVoxels;
    cdC.sizeVoxels = cloudDisplay.sizeVoxels;

    auto &cdP = cloudsD[(cloudsD.size()/2)  + idGrabber].display;
    cdP.cloudColor = cloudDisplay.cloudColor * 0.5f;
    cdP.sizePoints = cloudDisplay.sizePoints * 2.f;
    cdP.useVoxels  = cloudDisplay.useVoxels;
    cdP.sizeVoxels = cloudDisplay.sizeVoxels * 1.2f;

    m_redrawClouds = true;
}

auto K4CalibratorDrawer::update_settings(const DCCalibratorDrawerSettings &settings) -> void{
    m_settings = settings;
    m_redrawClouds = true;
}


