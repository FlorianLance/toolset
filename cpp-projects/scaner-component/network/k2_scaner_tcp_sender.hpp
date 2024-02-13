/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// std
#include <memory>

// boost
#include <boost/asio.hpp>

// base
#include "thirdparty/sigslot/signal.hpp"
#include "network/network_types.hpp"
#include "depth-camera/impl/k2_types.hpp"

namespace tool::net{

class K2ScanerTcpSender {

public:

    K2ScanerTcpSender(size_t id, std::vector<net::Interface> *localInterfaces);
    ~K2ScanerTcpSender();

// slots
    // connection
    auto enable_writing(std::string tagetName, int writingPort, size_t readingInterface, int readingPort) -> bool;
    auto disable_writing() -> void;

    // commands
    auto open_camera_command(cam::K2FrameRequest mode) -> void;
    auto close_camera_command() -> void;
    auto update_settings_command(cam::K2Settings parameters) -> void;
    auto ask_new_frame_command() -> void;
    auto set_size_udp_packets(int size) -> void;
    auto reading_interface() const -> size_t;

// signals
    sigslot::signal<bool> connection_state_signal;
    sigslot::signal<std::string> message_signal;
    sigslot::signal<std::string> error_signal;

private :

    auto init_command(int targetWritingPort, std::string targetWritingIpAddress) -> void;
    auto clean_socket() -> void;
    auto write_data() -> void;

private:

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}

