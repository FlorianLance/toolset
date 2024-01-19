/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#pragma once

// std
#include <atomic>

// base
#include "thirdparty/sigslot/signal.hpp"
#include "camera/impl/k2_network.hpp"

namespace tool{

class K2ProcessData{

public :

    K2ProcessData(size_t id);
    ~K2ProcessData();

    camera::K2CloudDisplayData *cloud_data();
    camera::K2MeshDisplayData *mesh_data();

// slots
    auto process_data_from_udp_frame(camera::K2UdpHeader header, std::shared_ptr<camera::K2Frame> frame) -> void;

// signals
    sigslot::signal<std::string> stack_trace_message_signal;
    sigslot::signal<std::int64_t, std::int64_t> end_processing_signal;

    // # images
    sigslot::signal<camera::K2CloudDisplayData *> update_cloud_data_signal;
    sigslot::signal<camera::K2MeshDisplayData *> update_mesh_data_signal;
    sigslot::signal<camera::K2BodiesDisplayData *> update_bodies_data_signal;

private :

    auto process_depth() -> void;
    auto process_infra() -> void;
    auto process_compressed_color_1920x1080() -> void;
    auto process_compressed_color_512x424() -> void;
    auto process_compressed_color_cloud() -> void;
    auto process_compressed_color_mesh() -> void;

public :

    std::atomic_bool isProcessing = false;

private :

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}
