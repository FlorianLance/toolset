
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
#include "utility/settings.hpp"
#include "geometry/point3.hpp"
#include "geometry/obb3.hpp"
#include "image/rgb.hpp"
#include "depth-camera/dc_enums.hpp"


namespace tool::cam {

enum class PlaneFilteringMode : std::int8_t{
    None,
    Above,
    Below
};

struct DCFiltersSettings : Settings{

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
    img::ColorRGB32 filterColor  = img::ColorRGB32{0.f,0.5f,0.08f};
    geo::Pt3f maxDiffColor  = geo::Pt3f{20.f,0.5f,0.5f};
    // # from infra
    bool filterDepthWithInfra        = false;
    // # from body tracking
    bool filterDepthWithBodyTracking = false;
    // # from cloud
    bool filterDepthWithCloud = false;
    PlaneFilteringMode p1FMode = PlaneFilteringMode::None;
    geo::Pt3f p1A = {1.f,1.f,0.f};
    geo::Pt3f p1B = {1.f,0.f,1.f};
    geo::Pt3f p1C = {1.f,0.f,0.f};
    bool removeFromPointDistance = false;
    geo::Pt3f pSphere = {0.f,0.f,1.f};
    float maxSphereDistance = 1000.0f;
    bool keepOnlyPointsInsideOOB = false;
    geo::OBB3<float> oob;
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

    DCFiltersSettings(){
        sType   = SettingsType::Filters;
        version = SettingsVersion::LastVersion;
    }
    DCFiltersSettings(std::span<const std::uint8_t> jsonBinary){
        DCFiltersSettings::init_from_json_binary(jsonBinary);
    }
    static auto default_init_for_calibration() -> DCFiltersSettings;

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    // legacy
    auto init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void override;
};

}

