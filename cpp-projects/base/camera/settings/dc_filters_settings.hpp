
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
#include "io/binary_settings.hpp"
#include "graphics/color.hpp"
// #include "utility/bit_mask.hpp"
// #include "camera/dc_enums.hpp"

namespace tool::camera {

struct DCFiltersSettings : io::BinaryFileSettings{

    enum class PlaneFilteringMode : std::int8_t{
        None,
        Above,
        Below
    };

    // width / height / depth
    float minWidthF  = 0.f;
    float maxWidthF  = 1.f;
    float minHeightF = 0.f;
    float maxHeightF = 1.f;
    float minDepthF  = 0.f;
    float maxDepthF  = 1.f;

    // color
    ColorRGB32 filterColor  = ColorRGB32{0.f,0.5f,0.08f};
    geo::Pt3f maxDiffColor  = geo::Pt3f{20.f,0.5f,0.5f};

    // compression
    unsigned char jpegCompressionRate = 80;

    // # neigbhours
    float maxLocalDiff = 10.f;
    unsigned char nbMinNeighbours = 1;

    // loops
    unsigned char minNeighboursLoops = 1;
    unsigned char erosionLoops = 1;

    // flogs
    bool filterDepthWithColor       = false;
    bool doLocalDiffFiltering       = true;
    bool doMinNeighboursFiltering   = false;
    bool doErosion                  = false;
    bool keepOnlyBiggestCluster     = false;
    bool invalidateColorFromDepth   = false;
    bool invalidateInfraFromDepth   = false;       

    PlaneFilteringMode p1FMode = PlaneFilteringMode::None;
    geo::Pt3f p1Pos;
    geo::Pt3f p1Rot;

    PlaneFilteringMode p2FMode = PlaneFilteringMode::None;
    geo::Pt3f p2Pos;
    geo::Pt3f p2Rot;


    DCFiltersSettings();
    DCFiltersSettings(std::int8_t const * const data, size_t &offset, size_t sizeData){
        DCFiltersSettings::init_from_data(data, offset, sizeData);
    }
    static auto default_init_for_calibration() -> DCFiltersSettings;

    // i/o
    auto init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void override;
    auto write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void override;
    auto total_data_size() const noexcept -> size_t override;
};

}
