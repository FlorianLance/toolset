
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

// kinect4
#include <k4a/k4atypes.h>

// local
#include "camera/dc_compressed_frame.hpp"

namespace tool::camera{


struct K4CompressedFrame : public DCCompressedFrame{

    // # calibration
    std::optional<k4a_calibration_t> calibration = std::nullopt;

    // # bodies
    // ...

    auto calibration_size() const noexcept -> size_t{
        return (calibration.has_value() ? sizeof(k4a_calibration_t) : 0) +
            sizeof(bool); // has calibration
    }

    auto bodies_size() const noexcept -> size_t{
        return sizeof(bool); // has body // TODO
    }

    auto total_data_size() const  -> size_t override{
        return
            DCCompressedFrame::total_data_size() +
            calibration_size() + bodies_size();
    }

    // file stream
    auto init_from_file_stream(std::ifstream &file) -> void override;
    auto write_to_file_stream(std::ofstream &file) -> void override;
    // # legacy
    auto init_legacy_cloud_frame_file_stream(std::ifstream &file) -> void;
    auto init_legacy_full_frame_file_stream(std::ifstream &file) -> void;
    // data
    auto init_from_data(std::int8_t *data) -> void override;
    auto convert_to_data(std::vector<std::int8_t> &data) -> size_t override;
};
}
