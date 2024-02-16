
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
#include "depth-camera/dc_enums.hpp"
// #include "utility/bit_mask.hpp"

namespace tool::cam {

enum class PlaneFilteringMode : std::int8_t{
    None,
    Above,
    Below
};


struct DCFiltersSettings : io::BinaryFileSettings{

    // depth filtering
    // # basic
    float minWidthF  = 0.f;
    float maxWidthF  = 1.f;
    float minHeightF = 0.f;
    float maxHeightF = 1.f;
    float minDepthF  = 0.f;
    float maxDepthF  = 1.f;
    // # from depth-sized color
    bool filterDepthWithColor = false;
    ColorRGB32 filterColor  = ColorRGB32{0.f,0.5f,0.08f};
    geo::Pt3f maxDiffColor  = geo::Pt3f{20.f,0.5f,0.5f};
    // # from infra
    bool filterDepthWithInfra        = false;
    // # from body tracking
    bool filterDepthWithBodyTracking = false;
    // # from cloud
    bool filterDepthWithCloud = false;
    PlaneFilteringMode p1FMode = PlaneFilteringMode::None;
    geo::Pt3f p1A;
    geo::Pt3f p1B;
    geo::Pt3f p1C;
    // # complex
    bool doLocalDiffFiltering = true;
    float maxLocalDiff = 10.f;
    DCConnectivity localDiffConnectivity = DCConnectivity::Connectivity_2H;
    // ## min neighbours
    bool doMinNeighboursFiltering = false;
    unsigned char nbMinNeighbours = 1;
    unsigned char minNeighboursLoops = 1;
    DCConnectivity minNeighboursConnectivity = DCConnectivity::Connectivity_4;
    // ## erosion
    bool doErosion = false;
    unsigned char erosionLoops = 1;
    DCConnectivity erosionConnectivity = DCConnectivity::Connectivity_8;
    // ## bigger cluster
    bool keepOnlyBiggestCluster = false;
    // ## remove after closest point
    bool removeAfterClosestPoint = false;
    float maxDistanceAfterClosestPoint = 0.2f;

    // depth-sized color filtering
    bool invalidateColorFromDepth   = false;

    // infra filtering
    bool invalidateInfraFromDepth   = false;

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





// struct DCFiltersSettings : io::BinaryFileSettings{

//     // width / height / depth
//     float minWidthF  = 0.f;
//     float maxWidthF  = 1.f;
//     float minHeightF = 0.f;
//     float maxHeightF = 1.f;
//     float minDepthF  = 0.f;
//     float maxDepthF  = 1.f;

//     // color
//     ColorRGB32 filterColor  = ColorRGB32{0.f,0.5f,0.08f};
//     geo::Pt3f maxDiffColor  = geo::Pt3f{20.f,0.5f,0.5f};

//     // # neigbhours
//     float maxLocalDiff = 10.f;
//     unsigned char nbMinNeighbours = 1;

//     // loops
//     unsigned char minNeighboursLoops = 1;
//     unsigned char erosionLoops = 1;

//     // flogs
//     bool filterDepthWithColor        = false;
//     bool filterDepthWithInfra        = false;
//     bool filterDepthWithBodyTracking = false;
//     bool filterDepthWithCloud        = false;

//     bool doLocalDiffFiltering       = true;
//     bool doMinNeighboursFiltering   = false;
//     bool doErosion                  = false;
//     bool keepOnlyBiggestCluster     = false;
//     bool invalidateColorFromDepth   = false;
//     bool invalidateInfraFromDepth   = false;

//     PlaneFilteringMode p1FMode = PlaneFilteringMode::None;
//     geo::Pt3f p1A;
//     geo::Pt3f p1B;
//     geo::Pt3f p1C;


//     DCFiltersSettings();
//     DCFiltersSettings(std::int8_t const * const data, size_t &offset, size_t sizeData){
//         DCFiltersSettings::init_from_data(data, offset, sizeData);
//     }
//     static auto default_init_for_calibration() -> DCFiltersSettings;

//     // i/o
//     auto init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void override;
//     auto write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void override;
//     auto total_data_size() const noexcept -> size_t override;
// };
