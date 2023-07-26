
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
#include <vector>

// local
#include "k4_grabber_data_processing.hpp"
#include "k4_volumetric_video.hpp"

namespace tool::camera{

class K4ServerData{

public:

    ~K4ServerData();

    auto initialize(size_t nbGrabbers) -> void;
    auto clean() -> void;

    auto new_compressed_frame(size_t idC, std::shared_ptr<camera::K4CompressedFrame> frame) -> void;
    auto new_frame(size_t idC, std::shared_ptr<camera::K4Frame> frame) -> void;

    size_t nb_grabbers() const noexcept {return m_grabbersDataProcessing.size();}
    auto get_frame(size_t idC) -> std::shared_ptr<camera::K4Frame>;
    auto get_compressed_frame(size_t idC) -> std::shared_ptr<camera::K4CompressedFrame>;

    auto invalid_last_frame(size_t idC) -> void;
    auto invalid_last_compressed_frame(size_t idC) -> void;

private:

    std::mutex m_recordLocker;
    std::atomic_bool m_record = false;
    std::vector<std::unique_ptr<camera::K4GrabberDataProcessing>> m_grabbersDataProcessing;
    camera::K4VolumetricVideo m_videoResource;
};
}
