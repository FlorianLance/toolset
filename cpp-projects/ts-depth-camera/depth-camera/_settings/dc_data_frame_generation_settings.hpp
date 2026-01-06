
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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

// json
#include <nlohmann/json_fwd.hpp>

// local
#include "depth-camera/dc_enums.hpp"

namespace tool::cam {

struct DCDataFrameGenerationSettings{

    // data to add
    bool addDepth              = true;
    bool addDepthSizedColor    = true;
    bool addOriginalSizeColor  = false;
    bool addInfra              = false;
    bool addBodiesId           = false;
    bool addBodiesSkeleton     = false;

    // TODO
    bool addCloud              = false; // TODO
    bool addAudio              = false; // TODO
    bool addImu                = false; // TODO

    // compression mode
    DCCompressionMode depthCM               = DCCompressionMode::FastPFor;
    DCCompressionMode depthSizedColorCM     = DCCompressionMode::JPEG;
    DCCompressionMode originalSizeColorCM   = DCCompressionMode::JPEG;
    DCCompressionMode infraCM               = DCCompressionMode::FastPFor;
    DCCompressionMode bodiesIdCM            = DCCompressionMode::None;

    DCCompressionMode cloudCM               = DCCompressionMode::FastPFor;
    std::uint8_t depthSizedColorJPEGCQ      = 95;
    std::uint8_t originalSizeColorJPEGCQ    = 95;

    // reconstruction
    CloudColorMode cloudColorMode = CloudColorMode::FromDepthSizedColorImage;

    [[nodiscard]] constexpr auto has_data() const noexcept -> bool{
        return  addOriginalSizeColor || addDepth || addDepthSizedColor ||
               addInfra || addBodiesId || addBodiesSkeleton || addAudio || addImu || addCloud;
    }

    auto init_from_json(const nlohmann::json &json) -> void;
    auto convert_to_json() const -> nlohmann::json;
};


}
