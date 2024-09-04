
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
#include "depth-camera/frame/dc_compressed_frame.hpp"
#include "depth-camera/frame/dc_frame.hpp"
#include "depth-camera/settings/dc_device_settings.hpp"

namespace tool::cam{

class DCClientProcessing{

public:

    DCClientProcessing();
    ~DCClientProcessing();

    // init / clean
    auto initialize(size_t nbDevices, bool startProcessingThread) -> void;
    auto clean() -> void;

    // modify
    // # device
    auto add_device_processor(bool startProcessingThread) -> void;
    auto remove_device_processor(size_t idD) -> void;
    auto update_device_settings(size_t idD, const cam::DCDeviceSettings &deviceS) -> void;
    // # frames
    auto new_compressed_frame(size_t idD, std::shared_ptr<cam::DCCompressedFrame> frame) -> void;
    auto new_frame(size_t idD, std::shared_ptr<cam::DCFrame> frame) -> void;
    auto invalid_last_frame(size_t idD) -> void;
    auto invalid_last_compressed_frame(size_t idD) -> void;
    // when no processing thread started
    auto process(size_t idD) -> void;
    auto uncompress_frame(size_t idD, std::shared_ptr<DCCompressedFrame> frame) -> std::shared_ptr<DCFrame>;

    // get
    auto nb_devices() const noexcept -> size_t;
    auto get_frame(size_t idD) -> std::shared_ptr<cam::DCFrame>;
    auto get_compressed_frame(size_t idD) -> std::shared_ptr<cam::DCCompressedFrame>;

    // I/O
    auto save_current_cloud(size_t idD, const std::string &path) -> bool;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
