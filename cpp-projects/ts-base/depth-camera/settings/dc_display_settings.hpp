
/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include "geometry/point4.hpp"

namespace tool::cam {

struct DCSceneDisplaySettings{

    geo::Pt4f backgroundColor = {0.83f, 0.84f, 0.81f, 1.f};
    bool drawGrid = true;

    // local
    int drawOnlyCloudId = -1;
};

struct DCDeviceDisplaySettings{

    bool showCapture = true;
    bool showCameraFrustum = true;
    bool showFilteringGizmos = true;
    bool showBodyTracking = true;

    // colors
    bool forceColor = false;
    float factorUnicolor = 0.65f;
    geo::Pt4f unicolor = {1.f,0.f,0.f, 1.f};

    // geometry
    bool backFaceCulling = false;
    bool circles = true; // if no points will be used
    // # points
    float pointSize = 5.f;
    // # circles
    float squareSize   = 0.0025f;
    float circleRadius = 0.25f;
};

}
