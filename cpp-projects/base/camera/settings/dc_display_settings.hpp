
/*******************************************************************************
** Toolset-base                                                               **
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
//#include "io/binary_settings.hpp"

namespace tool::camera {

struct DCSceneDisplaySettings{
    geo::Pt4f backgroundColor = {0.83f, 0.84f, 0.81f, 1.f};

    // local
    int drawOnlyCloudId = -1;

    // i/o
    static auto default_init_for_grabber() -> DCSceneDisplaySettings;
    static auto default_init_for_manager() -> DCSceneDisplaySettings;
};

struct DCCloudDisplaySettings{
    bool cloudVisible = true;
    bool forceCloudColor = false;
    geo::Pt4f cloudColor = {1.f,0.f,0.f, 1.f};
    float factorUnicolor = 0.65f;
    bool useVoxels = false;
    float sizePoints = 5.f;
    float sizeVoxels = 0.002f;

    // i/o
    static auto default_init_for_grabber() -> DCCloudDisplaySettings;
    static auto default_init_for_manager() -> DCCloudDisplaySettings;
};

}
