

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "dc_device_data.hpp"

using namespace tool::cam;

auto DCDeviceData::reset_spans() -> void{
    rawColor            = {};
    originalSizeColor   = {};
    depthSizedColor     = {};
    depth               = {};
    infra               = {};
    bodiesId            = {};
    depthCloud          = {};    
    binaryIMU           = {};
    audioChannels       = {0,{}};
    bodies              = {};
}

auto DCDeviceData::reset(bool hasDepth, size_t depthSize, bool hasColor, size_t colorWidth, size_t colorHeight) -> void{

    binaryCalibration   = {};
    reset_spans();

    // processsing
    if(hasDepth){
        depthMask.resize(depthSize);
        filteringMask.resize(depthSize);
        depthFiltering.resize(depthSize);
        zonesId.resize(depthSize);
    }else{
        depthMask.clear();
        filteringMask.clear();
        depthFiltering.clear();
        zonesId.clear();
    }

    if(hasColor){
        convertedColorData.resize(colorWidth*colorHeight);
        convertedColorData.width  = colorWidth;
        convertedColorData.height = colorHeight;
    }else{
        convertedColorData.reset();
    }

    if(hasColor && hasDepth){
        hsvDiffMask.resize(depthSize);
    }
}

