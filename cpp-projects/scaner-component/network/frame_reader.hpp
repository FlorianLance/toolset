/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// std
#include <mutex>
#include <atomic>

// base
#include "thirdparty/sigslot/signal.hpp"
#include "camera/kinect2/k2_network.hpp"

namespace tool::network{

class FrameReader;
using FrameReaderUP = std::unique_ptr<FrameReader>;

class FrameReader{

public:

    FrameReader();
    ~FrameReader();

// slots:
    void process_packets(std::vector<std::vector<char>*> *data);

// signals
    sigslot::signal<camera::K2UdpHeader, std::shared_ptr<camera::K2Frame>> frame_received_signal;
    sigslot::signal<std::string> stack_trace_message_signal;

private:

    struct Impl;
    std::unique_ptr<Impl> m_p;

};
}
