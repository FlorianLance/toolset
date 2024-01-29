
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

#pragma once

// k4a
#include "k4a/k4a.hpp"

// local
#include "camera/dc_device_impl.hpp"
#include "orbbec_device.hpp"

namespace tool::cam {

struct FemtoMegaDeviceImpl : public DCDeviceImpl{

    FemtoMegaDeviceImpl();

    // actions
    auto open(uint32_t deviceId) -> bool override;
    auto start_reading(const DCConfigSettings &newConfigS) -> bool override;
    auto stop_reading() -> void override;
    auto close() -> void override;

    // getters
    auto is_opened() const noexcept -> bool override;
    auto nb_devices() const noexcept -> std::uint32_t override;
    auto device_name() const noexcept -> std::string override;

private:

    // initialization
    auto initialize_device_specific() -> void override;
    auto update_camera_from_colors_settings() -> void override;

    // get data
    auto bodies_index_data() -> std::span<std::uint8_t> override{return {};} // TODO

    // read data
    auto capture_frame(std::int32_t timeoutMs) -> bool override;
    auto read_color_image() -> bool override;
    auto read_depth_image() -> bool override;
    auto read_infra_image() -> bool override;
    auto read_bodies() -> void override {} // TODO

    // process data
    auto convert_color_image() -> void override;
    auto resize_color_image_to_depth_size() -> void override;
    auto generate_cloud() -> void override;
    auto filter_cloud_image(const DCFiltersSettings &filtersS) -> void{} // TODO

    // frame generation
    auto create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame> override;

    std::unique_ptr<OrbbecDevice> orbbecD = nullptr;

    k4a::calibration k4aCalibration;
    k4a::transformation k4aTransformation;

    k4a::image k4aDepthSizedColorImage = nullptr;
    k4a::image k4aPointCloudImage = nullptr;

};


}
