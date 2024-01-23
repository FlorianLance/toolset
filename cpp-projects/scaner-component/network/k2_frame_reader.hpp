/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// base
#include "thirdparty/sigslot/signal.hpp"
#include "camera/impl/k2_network.hpp"

namespace tool::net{

class K2FrameReader{
public:
    K2FrameReader();
    ~K2FrameReader();

// slots:
    auto process_packets(std::vector<std::vector<char>*> *data) -> void;

// signals
    sigslot::signal<cam::K2UdpHeader, std::shared_ptr<cam::K2Frame>> frame_received_signal;
    sigslot::signal<std::string> stack_trace_message_signal;

private:
    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}
