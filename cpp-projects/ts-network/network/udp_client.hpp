
/*******************************************************************************
** Toolset-ts-network                                                         **
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

#pragma once

// local
#include "udp_reader.hpp"
#include "udp_sender.hpp"

namespace tool::net {

class UdpClient{
public:

    UdpClient(){
        messagesBuffer.resize(100, 0);
    }

    ~UdpClient(){
        clean();
    }

    auto initialize(const UdpServerInfo &infos) -> bool{
        return false;
    }

    auto clean() -> void{

    }

protected:

    std::unique_ptr<UdpReader> m_reader = nullptr;
    std::unique_ptr<UdpSender> m_sender = nullptr;

    // std::atomic_bool sendMessages = false;
    // std::unique_ptr<std::thread> sendMessagesT = nullptr;

    DoubleRingBuffer<std::byte> messagesBuffer;
};



}
