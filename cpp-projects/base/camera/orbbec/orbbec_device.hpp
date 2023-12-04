
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
#include <memory>
#include <string>

// local
#include "camera/dc_device.hpp"
#include "files/binary_settings.hpp"

namespace tool::camera {

//struct OrbbecDeviceConfigSettings{// : files::SubBinarySettings{

//    std::uint32_t deviceId = 0;

////    auto init_from_data(std::int8_t *data) -> void override;
////    auto convert_to_data(std::int8_t *data) const -> void override;
////    auto total_data_size() const noexcept-> size_t override;
//};

class OrbbecDevice : public DCDevice{
public:

    OrbbecDevice();
    ~OrbbecDevice();

    auto open(std::uint32_t deviceId) -> bool override;
    auto refresh_devices_list() -> void  override;
    auto close() -> void  override {}
    auto clean() -> void override;

    // getters
    auto nb_devices() const noexcept -> std::uint32_t override;
    auto device_name() const noexcept-> std::string override;
    auto device_id() const noexcept -> std::uint32_t override;
    auto is_opened() const noexcept -> bool override;
    auto cameras_started() const noexcept -> bool override;
    // ...

    // # config
    auto start_cameras(const DCConfigSettings &configS) -> bool override;
    // ...

    // cameras
    // ...

    // settings
    auto set_color_settings(const DCColorSettings &colorS) -> void override;


private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
