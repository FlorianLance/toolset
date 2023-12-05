
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
#include "thirdparty/sigslot/signal.hpp"

namespace tool::network{


class UdpReader {

public:

    UdpReader();
    ~UdpReader();

    // slots
    auto init_socket(std::string readingAdress, int readingPort) -> bool;
    auto clean_socket() -> void;

    // reading thread
    auto start_reading() -> void;
    auto stop_reading() -> void;

    auto is_reading() const noexcept -> bool;
    auto is_connected() const noexcept -> bool;

    // signals
    sigslot::signal<bool> connection_state_signal;
    sigslot::signal<> timeout_packet_signal;

protected:

    virtual auto process_packet(std::vector<char> *packet, size_t nbBytes) -> void;

    std::vector<std::int8_t> m_data;

private :

    auto read_data() -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
