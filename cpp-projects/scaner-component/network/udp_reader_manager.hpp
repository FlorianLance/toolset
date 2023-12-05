/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// std
#include <memory>

// base
#include "thirdparty/sigslot/signal.hpp"
#include "network/network_types.hpp"

namespace tool::network{

class UdpReaderManager;
using UdpReaderManagerUP = std::unique_ptr<UdpReaderManager>;

class UdpReaderManager{

public:

    UdpReaderManager(const std::vector<Interface> &interfaces);
    ~UdpReaderManager();

    auto enable_reading() -> bool;
    auto disable_reading() -> void;

// signals
    sigslot::signal<std::string > message_signal;
    sigslot::signal<std::string > error_signal;
    sigslot::signal<size_t, bool> connected_state_signal;
    sigslot::signal<size_t> timeout_signal;
    sigslot::signal<size_t, size_t, size_t> packet_received_signal;
    sigslot::signal<size_t, size_t, size_t, size_t> frame_dropped_signal;
    sigslot::signal<size_t, size_t, std::vector<std::vector<char>*> *> all_packets_received_signal;


private:

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}

