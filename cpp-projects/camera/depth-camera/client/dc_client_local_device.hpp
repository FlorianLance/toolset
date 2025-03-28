
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
#include "depth-camera/frame/dc_frame.hpp"
#include "depth-camera/frame/dc_data_frame.hpp"
#include "dc_client_device.hpp"

namespace tool::cam {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class DCClientLocalDevice : public DCClientDevice{
public:

    DCClientLocalDevice();
    ~DCClientLocalDevice() override;
    
    auto initialize(const DCDeviceConnectionSettings &connectionS) -> bool override;
    auto clean() -> void override;

    // when device thread not started
    auto read_frames_from_external_thread() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>;

    auto update_device_settings(const cam::DCDeviceSettings &deviceS) -> void override;
    auto update_color_settings(const cam::DCColorSettings &colorS) -> void override;
    auto update_filters_settings(const cam::DCFiltersSettings &filtersS) -> void override;
    auto update_misc_settings(const cam::DCMiscSettings &delayS) -> void override;

    constexpr auto type() const noexcept -> DCClientType override {return DCClientType::Local;}
    auto device_connected() const noexcept -> bool override;

    // signals
    sigslot::signal<size_t, std::shared_ptr<cam::DCFrame>> local_frame_signal;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};


}
