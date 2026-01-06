
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

// std
#include <vector>
#include <chrono>

namespace tool::cam {

struct DCCalibratorStates{

    // # time
    bool isRegistering = false;
    std::chrono::milliseconds startTime = std::chrono::milliseconds(0);
    std::chrono::milliseconds elapsedTime = std::chrono::milliseconds(0);
    std::vector<size_t> nbFramesRegistered;

    // # actions
    bool recomputeRegisteringProcessing = false;
    bool resetCalibration               = false;
    bool startCalibration               = false;
    bool stopCalibration                = false;
    bool calibrate                      = false;
    bool validateCalibration            = false;
    bool updateDisplaySettings          = false;
    bool updateFilteringMode            = false;
    int filteringMode                   = 0;

    auto reset_actions() -> void{
        recomputeRegisteringProcessing = false;
        resetCalibration               = false;
        startCalibration               = false;
        stopCalibration                = false;
        calibrate                      = false;
        validateCalibration            = false;
        updateDisplaySettings          = false;
        updateFilteringMode            = false;
    }
};
}
