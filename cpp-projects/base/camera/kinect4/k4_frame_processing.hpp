
/*******************************************************************************
** Toolbox-base                                                               **
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
#include "k4_frame_uncompressor.hpp"

namespace tool::camera{

struct K4FramesProcessing{

    K4FramesProcessing();
    ~K4FramesProcessing(){
        clean();
    }

    auto start() -> void;
    auto stop() -> void;
    auto clean() -> void;

    auto new_compressed_frame(std::shared_ptr<K4CompressedFrame> frame) -> void;
    auto new_frame(std::shared_ptr<K4Frame> frame) -> void;
    auto get_frame() -> std::shared_ptr<K4Frame>;
    auto invalid_frame() -> void;

private:

    auto process() -> void;

    std::atomic_bool m_isProcessing       = false;
    std::unique_ptr<std::thread> m_thread = nullptr;
    std::unique_ptr<std::mutex> m_locker  = nullptr;

    std::shared_ptr<K4CompressedFrame> m_lastCF = nullptr;
    std::shared_ptr<K4Frame> m_lastF = nullptr;
    std::shared_ptr<K4Frame> m_frame = nullptr;

    std::unique_ptr<K4FrameUncompressor> m_frameUncompressor;
};
}
