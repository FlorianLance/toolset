/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// base
#include "thirdparty/sigslot/signal.hpp"
#include "network/network_types.hpp"

namespace tool::net{

class K2ScanerUdpReader {

public:

    K2ScanerUdpReader(size_t id, tool::net::Interface interface);
    ~K2ScanerUdpReader();

    size_t id() const;
    tool::net::Interface interface() const;

// slots
    auto enable_reading(int readingPort) -> bool;
    auto disable_reading() -> void;
    auto read_data() -> void;

private:

    auto clean_socket() -> void;

// signals
public:

    sigslot::signal<> timeout_packet_signal;
    sigslot::signal<bool> connected_state_signal;
    sigslot::signal<std::string> message_signal;
    sigslot::signal<std::string> error_signal;

    sigslot::signal<size_t,size_t,size_t> packet_received_signal;
    sigslot::signal<size_t,size_t,size_t,size_t> frame_dropped_signal;
    sigslot::signal<size_t,size_t ,std::vector<std::vector<char>*> *> all_packets_received_signal;


private :

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}
