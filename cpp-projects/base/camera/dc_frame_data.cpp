

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

#include "dc_frame_data.hpp"

using namespace tool::cam;

auto DCFrameData::reset(const DCModeInfos &mInfos) -> void{

    rawColor        = {};
    color           = {};
    dephtSizedColor = {};
    depth           = {};
    infra           = {};
    depthCloud      = {};
    bodiesIdDepth   = {};
    audioChannels   = {0,{}};
    imuSample       = std::nullopt;
    bodies.clear();

    if(mInfos.has_depth()){
        depthMask.resize(mInfos.depth_size());
        filteringMask.resize(mInfos.depth_size());
        depthFiltering.resize(mInfos.depth_size());
        zonesId.resize(mInfos.depth_size());
    }else{
        depthMask.clear();
        filteringMask.clear();
        depthFiltering.clear();
        zonesId.clear();
    }

    if(mInfos.has_color()){
        convertedColorData.resize(mInfos.color_size());
        convertedColorData.width  = mInfos.color_width();
        convertedColorData.height = mInfos.color_height();
    }else{
        convertedColorData.reset();
    }
}
