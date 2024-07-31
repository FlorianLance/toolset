
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

#pragma once

// local
#include "network/settings/udp_server_settings.hpp"

#include "udp_reader.hpp"
#include "udp_sender.hpp"

namespace tool::net {

struct UdpMessageSender{

    std::unique_ptr<UdpSender> sender = nullptr;
    std::unique_ptr<std::thread> sendMessagesT = nullptr;

    auto initialize(const UdpConnectionSettings &udpConnectionS) -> void;

    auto send_message() -> void{

    }
};


class UdpServer{
public:


    UdpServer();
    virtual ~UdpServer();

    auto start_reading(const UdpServerSettings &udpServerS) -> bool;
    auto stop_reading() -> void{}

    auto add_client(const UdpConnectionSettings &udpConnectionS) -> void;

    auto clean() -> void;

protected:
    auto initialize() -> void;

    virtual auto init_connections() -> void{

    }

    auto send_message_loop() -> void{

    }

    virtual auto generate_sender() -> std::unique_ptr<UdpSender> = 0;
    virtual auto generate_reader() -> std::unique_ptr<UdpReader> = 0;

protected:

    // reading
    std::unique_ptr<UdpReader> m_reader = nullptr;

    // sending
    std::atomic_bool m_sendMessages = false;
    std::vector<UdpMessageSender> m_clientsSenders;

    struct Impl;
    std::unique_ptr<Impl> i;
};



}
