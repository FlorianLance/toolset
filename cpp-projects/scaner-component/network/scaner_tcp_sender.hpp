/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// std
#include <memory>
#include <chrono>

// boost
#include <boost/asio.hpp>

// base
#include "thirdparty/sigslot/signal.hpp"
#include "camera/kinect2/k2_network.hpp"
#include "network/network_types.hpp"

namespace tool::network{

class ScanerTcpSender;
using TcpSenderUP = std::unique_ptr<ScanerTcpSender>;

class ScanerTcpSender {

public:

    ScanerTcpSender(size_t id, std::vector<network::Interface> *localInterfaces);
    ~ScanerTcpSender();

// slots

    // connection
    bool enable_writing(std::string tagetName, int writingPort, size_t readingInterface, int readingPort);
    void disable_writing();

    // commands
    void open_camera_command(camera::K2FrameRequest mode);
    void close_camera_command();
    void update_settings_command(camera::K2Settings parameters);
    void ask_new_frame_command();
    void set_size_udp_packets(int size);
    size_t reading_interface() const;

// signals

    sigslot::signal<bool> connection_state_signal;
    sigslot::signal<std::string> message_signal;
    sigslot::signal<std::string> error_signal;

private :

    void init_command(int targetWritingPort, std::string targetWritingIpAddress);
    void clean_socket();
    void write_data();

private:

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}

