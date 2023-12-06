
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
//#include <cstdint>

// local
#include "dc_types.hpp"
#include "geometry/cloud.hpp"

namespace tool::camera{

struct Frame{
    std::int32_t idCapture = 0;
    std::int64_t afterCaptureTS = 0;    
    virtual ~Frame(){}
};

struct DCFrame : Frame{

    // info
    DCMode mode;

    // color
    size_t colorWidth = 0;
    size_t colorHeight = 0;
    std::vector<geo::Pt4<std::uint8_t>> imageColorData;

    // depth sized color
    size_t depthSizedColorWidth = 0;
    size_t depthSizedColorHeight = 0;
    std::vector<geo::Pt4<std::uint8_t>> depthSizedImageColorData;

    // depth image
    size_t depthWidth = 0;
    size_t depthHeight = 0;
    std::vector<std::uint16_t> depthData;
    std::vector<geo::Pt3<std::uint8_t>> imageDepthData;

    // infra
    size_t infraWidth = 0;
    size_t infraHeight = 0;
    std::vector<std::uint16_t> infraData;
    std::vector<geo::Pt3<std::uint8_t>> imageInfraData;

    // cloud
    geo::ColoredCloudData cloud;

    // imu
    std::optional<DCImuSample> imuSample;

    // audio
    std::vector<std::array<float, 7>> audioFrames;

    // bodies
    std::vector<DCBody> bodies;

//    virtual auto copy(const DCFrame &source) -> void{}
};
}
