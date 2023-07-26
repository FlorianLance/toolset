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
#include "camera/kinect2/k2_network.hpp"

namespace tool{

class ProcessData;
using ProcessDataUP = std::unique_ptr<ProcessData>;

class ProcessData{

public :

    ProcessData(size_t id);
    ~ProcessData();

    camera::K2CloudDisplayData *cloud_data();
    camera::K2MeshDisplayData *mesh_data();

// slots
    void process_data_from_udp_frame(camera::K2UdpHeader header, std::shared_ptr<camera::K2Frame> frame);

// signals
    sigslot::signal<std::string> stack_trace_message_signal;
    sigslot::signal<std::int64_t, std::int64_t> end_processing_signal;

    // # images
    sigslot::signal<camera::K2CloudDisplayData *> update_cloud_data_signal;
    sigslot::signal<camera::K2MeshDisplayData *> update_mesh_data_signal;
    sigslot::signal<camera::K2BodiesDisplayData *> update_bodies_data_signal;

private :

    void process_depth();
    void process_infra();
    void process_compressed_color_1920x1080();
    void process_compressed_color_512x424();
    void process_compressed_color_cloud();
    void process_compressed_color_mesh();

public :

    std::atomic_bool isProcessing = false;

private :

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}
