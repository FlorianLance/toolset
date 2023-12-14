
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

// std
#include <vector>

// local
//#include "io/binary_settings.hpp"

namespace tool::camera {

struct DCRecorderStates{

    bool isRecording = false;
    std::vector<size_t> nbFramesRecorded;
    std::vector<size_t> currentFrames;
    double currentTime = 0.0;
    double duration = 0.0;

    // actions
    bool startRecording = false;
    bool stopRecording  = false;
    bool resetRecording = false;
    bool moveTime       = false;

    auto reset_actions() -> void{
        startRecording = false;
        stopRecording  = false;
        resetRecording = false;
        moveTime       = false;
    }
};

struct DCRecorderSettings{
    int cameraMaxFramesToRecord = 10000;
    double maxDurationS = 500.;
    // output
    std::vector<bool> camerasToRecord;
    bool recordAllData = true;
    bool recordColor   = true;
    bool recordDepth   = true;
    bool recordInfra   = true;
    bool recordCloud   = true;
    bool recordIMU     = true;
    bool recordAudio   = true;
    bool recordBodies  = true;
};
}
