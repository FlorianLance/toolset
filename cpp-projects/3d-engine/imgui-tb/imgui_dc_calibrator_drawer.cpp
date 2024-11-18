
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

#include "imgui_dc_calibrator_drawer.hpp"

using namespace tool::cam;
using namespace tool::graphics;

auto DCCalibratorDrawer::initialize(size_t nbClients) -> void{

    DCCloudsSceneDrawer::initialize(2*nbClients);

    for(size_t ii = 0; ii < nbClients; ++ii){
        auto &cdC = cloudsD[ii]->display;
        cdC.forceColor = true;
        auto &cdP = cloudsD[nbClients  + ii]->display;
        cdP.forceColor = true;
    }
}

auto DCCalibratorDrawer::set_data(int sourceId, int modelId, std::vector<cam::DCCalibratorClientData>* clientsData) -> void{

    auto allSelection = modelId == clientsData->size();
    for(size_t ii = 0; ii < clientsData->size(); ++ii){

        auto clientData = &(*clientsData)[ii];

        // calibration
        update_from_colored_cloud_data(clientData->id, clientData->calibrationCloud);

        // processsed
        update_from_colored_cloud_data(clientsData->size() + clientData->id, clientData->processedCloud);

        auto visibilty = (ii == modelId) || (ii == sourceId) || allSelection;

        auto &cdC = cloudsD[ii]->display;
        cdC.showCapture = m_settings.displayCalibrationCloud && visibilty;

        auto &cdP = cloudsD[clientsData->size()  + ii]->display;
        cdP.showCapture = m_settings.displayProcessedCloud && visibilty;
    }
    m_redrawClouds = true;
}

auto DCCalibratorDrawer::draw() -> void{

    if(m_redrawClouds || has_to_redraw_clouds()){
        draw_clouds_to_fbo();
    }
    m_redrawClouds = false;

    draw_all_clouds_drawers_in_one_tab(false, true, "Calibration clouds");
}

auto DCCalibratorDrawer::update_client_model(size_t idClient, const cam::DCModelSettings &model) -> void{

    auto tr = model.compute_full_transformation();
    cloudsD[idClient]->model                      = tr;
    cloudsD[(cloudsD.size()/2) + idClient]->model = tr;
    m_redrawClouds = true;
}

auto DCCalibratorDrawer::update_client_cloud_display(size_t idClient, const DCDeviceDisplaySettings &cloudDisplay) -> void{

    auto &cdC = cloudsD[idClient]->display;
    cdC.forceColor          = true;
    cdC.backFaceCulling     = false;
    cdC.unicolor            = cloudDisplay.unicolor;
    cdC.circles             = cloudDisplay.circles;
    cdC.pointSize           = cloudDisplay.pointSize;
    cdC.squareSize          = cloudDisplay.squareSize;
    cdC.circleRadius        = cloudDisplay.circleRadius;
    cdC.showCameraFrustum   = false;
    cdC.showFilteringGizmos = false;

    auto &cdP               = cloudsD[(cloudsD.size()/2)  + idClient]->display;
    cdP.forceColor          = true;
    cdP.backFaceCulling     = false;
    cdP.unicolor            = cdC.unicolor * 0.3f;
    cdP.pointSize           = cdC.pointSize * 2.f;
    cdP.circles             = cdC.circles;
    cdP.squareSize          = cdC.squareSize * 1.2f;
    cdP.circleRadius        = cdC.circleRadius * 1.2f;
    cdP.showCameraFrustum   = false;
    cdP.showFilteringGizmos = false;

    m_redrawClouds = true;
}

auto DCCalibratorDrawer::update_settings(const DCCalibratorDisplaySettings &settings) -> void{
    m_settings = settings;
    m_redrawClouds = true;
}


