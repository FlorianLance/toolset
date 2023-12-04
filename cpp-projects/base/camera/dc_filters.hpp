
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
#include "files/binary_settings.hpp"
#include "graphics/color.hpp"
#include "utility/bit_mask.hpp"
#include "dc_enums.hpp"

namespace tool::camera {

struct DCFilters : files::BinaryFileSettings{

    // # width / height
    unsigned int minWidth  = 0;
    unsigned int maxWidth  = depth_resolution(dcDefaultMode).x();
    unsigned int minHeight = 0;
    unsigned int maxHeight = depth_resolution(dcDefaultMode).y();

    // color
    float yFactor = 1.f; // deprecated
    float uFactor = 1.f; // deprecated
    float vFactor = 1.f; // deprecated

    ColorRGB32 filterColor  = ColorRGB32{0.f,0.5f,0.08f};
    geo::Pt3f maxDiffColor = geo::Pt3f{20.f,0.5f,0.5f};

    // # depth
    std::int16_t minDepthValue = static_cast<std::int16_t>(range(dcDefaultMode).x()*1000.f);
    std::int16_t maxDepthValue = static_cast<std::int16_t>(range(dcDefaultMode).y()*1000.f);

    // compression
    unsigned char jpegCompressionRate = 80;

    // # neigbhours
    float maxLocalDiff = 10.f;
    unsigned char nbMinNeighbours = 1;
    unsigned char minNeighboursLoops = 1;

    // # erosion
    unsigned char erosionLoops = 1;

    // flogs
    bool filterDepthWithColor       = false;
    bool doLocalDiffFiltering       = true;
    bool doMinNeighboursFiltering   = false;
    bool doErosion                  = false;
    bool keepOnlyBiggestCluster     = false;
    bool invalidateColorFromDepth   = false;
    bool invalidateInfraFromDepth   = false;

    enum class PlaneFilteringMode : std::int8_t{
        None,
        Above,
        Below
    };

    PlaneFilteringMode p1FMode = PlaneFilteringMode::None;
    geo::Pt3f p1Pos;
    geo::Pt3f p1Rot;

    PlaneFilteringMode p2FMode = PlaneFilteringMode::None;
    geo::Pt3f p2Pos;
    geo::Pt3f p2Rot;

    // masks
    static constexpr auto maskMaxRes  = camera::depth_resolution(camera::DCMode::Cloud_1024x1024);
    static constexpr auto maskMaxSize = maskMaxRes.x()*maskMaxRes.y();
    BitMask<maskMaxSize> depthMask = BitMask<maskMaxSize>(true);

    // local
    int idPencil = 0;

    DCFilters() = default;
    DCFilters(std::int8_t *data){DCFilters::init_from_data(data);}
    static auto default_init_for_calibration() -> DCFilters;

    // i/o
    auto init_from_data(std::int8_t *data) -> void override;
    auto convert_to_data(std::int8_t *data) const -> void override;

    auto total_data_size() const noexcept -> size_t override{
        return
            sizeof(unsigned int)*4+
            sizeof(float)*5+
            sizeof(geo::Pt3f)*2+
            sizeof(std::int16_t)*2+
            sizeof(unsigned char)*4 +
            sizeof(bool)*7 +
            //  planes test
            sizeof(geo::Pt3f)*4 +
            sizeof(std::int8_t) * 2
        ;
    }
    auto type() const noexcept  -> std::int32_t override {return static_cast<std::int32_t>(DCSettingsType::Filters);};
    auto file_description() const noexcept -> std::string_view override {return settings_name(static_cast<DCSettingsType>(type()));}
};

}
