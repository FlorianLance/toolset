
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

#include "frame.hpp"

// local
#include "utility/io_data.hpp"
#include "utility/io_fstream.hpp"

using namespace tool::cam;

auto Frame::data_size() const noexcept -> size_t{
    return
        sizeof(idDevice) +
        sizeof(idCapture) +
        sizeof(afterCaptureTS) +
        sizeof(receivedTS);
}

auto Frame::init_from_file_stream(std::ifstream &file) -> void{
    read(idDevice,        file);
    read(idCapture,       file);
    read(afterCaptureTS,  file);
    read(receivedTS,      file);
}

auto Frame::init_from_data(std::span<const std::byte> data, size_t &offset) -> void{
    read(idDevice,       data, offset);
    read(idCapture,       data, offset);
    read(afterCaptureTS,  data, offset);
    read(receivedTS,      data, offset);
}

auto Frame::write_to_file_stream(std::ofstream &file) -> void{
    write(idDevice,       file);
    write(idCapture,       file);
    write(afterCaptureTS,  file);
    write(receivedTS,      file);
}

auto Frame::write_to_data(std::span<std::byte> data, size_t &offset) -> void{
    write(idDevice,       data, offset);
    write(idCapture,       data, offset);
    write(afterCaptureTS,  data, offset);
    write(receivedTS,      data, offset);
}
