
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
#include "camera/dc_device_impl.hpp"
#include "azure_base_device.hpp"

namespace tool::cam {


struct AzureKinectDeviceImpl : public DCDeviceImpl{

    AzureKinectDeviceImpl();

    // actions
    auto open(std::uint32_t deviceId) -> bool override final;
    auto start_reading(const DCConfigSettings &newConfigS) -> bool override final;
    auto stop_reading() -> void override final;
    auto close() -> void override final;

    // getters
    auto is_opened() const noexcept -> bool override final;
    auto nb_devices() const noexcept -> std::uint32_t override final;
    auto device_name() const noexcept -> std::string override final;

private:

    // initialization
    auto initialize_device_specific() -> void override final;
    auto update_camera_from_colors_settings() -> void override final;

    // read data
    auto capture_frame(std::int32_t timeoutMs) -> bool override final;
    auto read_color_image() -> bool override final;
    auto read_depth_image() -> bool override final;
    auto read_infra_image() -> bool override final;
    auto read_from_microphones() -> void override final;
    auto read_from_imu() -> void override final;
    auto read_bodies() -> void override final;

    // process data    
    auto resize_color_image_to_depth_size() -> void override final;
    auto generate_cloud() -> void override final;

    // frame generation
    auto create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame> override final;
    auto compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame> override final;

    std::unique_ptr<AzureBaseDevice> azureD = nullptr;
};

}
