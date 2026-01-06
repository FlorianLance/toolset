
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
#include "depth-camera/frame/dc_frame.hpp"
#include "depth-camera/settings/dc_frame_generation_settings.hpp"

// local
#include "dc_data_frame.hpp"

namespace tool::cam{

struct DCFrameProcessor{

    DCFrameProcessor();
    ~DCFrameProcessor();

    // set
    auto new_data_frame(std::shared_ptr<DCDataFrame> frame) -> void;
    auto new_frame(std::shared_ptr<DCFrame> frame) -> void;
    auto invalid_frame() -> void;
    auto invalid_data_frame() -> void;
    auto update_generation_settings(const DCFrameGenerationSettings &generationS) -> void;

    // get
    auto get_frame() -> std::shared_ptr<DCFrame>;
    auto get_data_frame() -> std::shared_ptr<DCDataFrame>;

    // processing thread
    auto start_processing_thread() -> void;
    auto stop_processing_thread() -> void;
    auto clean_processing_thread() -> void;

    // processing
    auto process() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>;
    auto generate(std::shared_ptr<DCDataFrame> dFrame) -> std::shared_ptr<DCFrame>;

    std::atomic<double> ucUsage = 0.0;
    std::atomic<double> avegageProcessMs = 0.0;

private:

    auto process_thread() -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
