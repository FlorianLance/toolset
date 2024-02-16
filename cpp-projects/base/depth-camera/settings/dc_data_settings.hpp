
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

namespace tool::cam {

enum class CloudColorMode : std::int8_t{
    FromDepthData = 0,          // use a gradient level to generate colors corresponding to depth
    FromDepthSizedColorImage    // use colors from deph-sized color image
};

enum class CloudGenerationMode : std::int8_t{
    FromDepth = 0,
    FromDecodedCloud
};

struct DCCaptureDataSettings{
    bool color          = true;
    bool depth          = true;
    bool infra          = false;
    bool bodyTracking   = false;
    float btTemporalSmoothing = 0.f;
    bool audio          = false;
    bool imu            = false;    
};

struct DCFrameCompressionSettings{
    std::uint8_t jpegCompressionRate = 80;
    bool calibration        = true;
    bool depth              = true;
    bool depthSizedColor    = true;
    bool color              = false;
    bool infra              = false;
    bool bodyIdMap          = false;
    bool cloud              = false;
    bool bodyTracking       = false;
    bool audio              = false;
    bool imu                = false;
    CloudColorMode cloudColorMode = CloudColorMode::FromDepthSizedColorImage;

    [[nodiscard]] constexpr auto has_data_to_compress() const noexcept -> bool{
        return  calibration || color || depth || depthSizedColor || infra || bodyTracking || audio || imu || cloud;
    }
};

struct DCFrameGenerationSettings{

    bool calibration          = true;
    bool depth                = true;
    bool depthSizedColorImage = true;
    bool cloud                = true;
    bool infra                = false;
    bool colorImage           = false;
    bool depthImage           = false;
    bool infraImage           = false;
    bool bodyIdMapImage       = false;
    bool bodyTracking         = false;
    bool imu                  = false;
    bool audio                = false;
    CloudGenerationMode cloudGenMode = CloudGenerationMode::FromDepth;
    CloudColorMode cloudColorMode = CloudColorMode::FromDepthSizedColorImage;

    [[nodiscard]] constexpr auto has_data_to_generate() const noexcept -> bool{
        return  calibration || depth || infra || colorImage || depthSizedColorImage || depthImage || infraImage || cloud || imu || bodyTracking || audio;
    }
};

struct DCClientDataActions{
    DCCaptureDataSettings capture;
    DCFrameGenerationSettings generation;
    DCFrameCompressionSettings compression;
};

struct DCServerDataActions{
    DCFrameGenerationSettings generation;
};

struct DCDataSettings : io::BinaryFileSettings{

    [[nodiscard]] constexpr auto capture_color()              const noexcept -> bool{return client.capture.color;}
    [[nodiscard]] constexpr auto capture_depth()              const noexcept -> bool{return client.capture.depth;}
    [[nodiscard]] constexpr auto capture_infra()              const noexcept -> bool{return client.capture.infra;}
    [[nodiscard]] constexpr auto capture_body_tracking()      const noexcept -> bool{return client.capture.bodyTracking;}
    [[nodiscard]] constexpr auto capture_audio()              const noexcept -> bool{return client.capture.audio;}
    [[nodiscard]] constexpr auto capture_imu()                const noexcept -> bool{return client.capture.imu;}
    [[nodiscard]] constexpr auto generate_cloud_from_client() const noexcept             -> bool{return capture_depth() &&  (client.generation.cloud || client.compression.cloud);}

    DCClientDataActions client;
    DCServerDataActions server;

    DCDataSettings();
    DCDataSettings(std::int8_t const * const data, size_t &offset, size_t sizeData){
        DCDataSettings::init_from_data(data, offset, sizeData);
    }

    // i/o
    auto init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void override;
    auto write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void override;
    auto total_data_size() const noexcept-> size_t override;
};
}
