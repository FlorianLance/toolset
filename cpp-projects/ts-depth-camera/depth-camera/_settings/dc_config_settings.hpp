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

// base
#include "utility/settings.hpp"
#include "geometry/matrix3.hpp"
#include "geometry/point3.hpp"

// local
#include "depth-camera/dc_enums.hpp"

namespace tool::cam {

struct DCConfigSettings : Settings{
    
    // actions
    bool openDevice   = false;
    bool startReading = false;
    // device
    DCType typeDevice = DCType::AzureKinect;
    bool useSerialNumber = false;
    std::uint32_t idDevice = 0;
    std::string serialNumber = "";
    geo::Pt4<std::uint8_t> ipv4Address = {192,168,1,10};
    DCMode mode = dc_default_mode(typeDevice);
    // streams
    bool enableIRStream = false;
    // synch
    bool synchronizeColorAndDepth = true;
    int delayBetweenColorAndDepthUsec = 0;
    DCSynchronisationMode synchMode = DCSynchronisationMode::Standalone;
    int subordinateDelayUsec = 0;
    // body tracking
    bool btEnabled = false;
    DCBTSensorOrientation btOrientation = DCBTSensorOrientation::default_orientation;
    DCBTProcessingMode btProcessingMode = DCBTProcessingMode::GPU_DIRECTML;
    std::int8_t btGPUId = 0;
    // float btTemporalSmoothing = 0.f;
    // misc
    bool disableLED = false;
    // color - depth calibration
    tool::geo::Pt3f colorAlignmentTr = {0,0,0};
    tool::geo::Pt3f colorAlignmentRotEuler = {0,0,0};
    // tool::geo::Mat3f colorAlignmentRot = geo::Mat3f::identity();
    
    static auto default_init_for_server() -> DCConfigSettings;
    static auto default_init_for_client() -> DCConfigSettings;

    DCConfigSettings(){
        sType   = SettingsType::Device_config;
        version = SettingsVersion::LastVersion;
    }
    DCConfigSettings(std::span<const std::uint8_t> jsonBinary){
        DCConfigSettings::init_from_json_binary(jsonBinary);
    }

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    // legacy
    auto init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void override;
};



}
