/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// base
#include "thirdparty/sigslot/signal.hpp"
#include "network/network_utility.hpp"


namespace tool::network{

class ScanerUdpReader;
using ScanerUdpReaderUP = std::unique_ptr<ScanerUdpReader>;


class ScanerUdpReader {

public:

    ScanerUdpReader(size_t id, tool::network::Interface interface);
    ~ScanerUdpReader();

    size_t id() const;
    tool::network::Interface interface() const;

// slots

    bool enable_reading(int readingPort);
    void disable_reading();
    void read_data();

private:

    void clean_socket();

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
