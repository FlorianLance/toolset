// /*******************************************************************************
// ** exvr-scaner-component                                                      **
// ** No license (to be defined)                                                 **
// ** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
// ********************************************************************************/

// #pragma once

// // std
// #include <array>
// #include <memory>
// #include <thread>
// #include <mutex>
// #include <atomic>

// // base
// #include "thirdparty/sigslot/signal.hpp"
// #include "geometry/point4.hpp"
// #include "utility/io_file.hpp"
// #include "depth-camera/impl/k2_config_files.hpp"

// // local
// #include "network/k2_scaner_tcp_sender.hpp"
// #include "network/k2_udp_reader_manager.hpp"
// #include "network/k2_frame_reader.hpp"
// #include "k2_process_data.hpp"

// namespace tool::scan {

// class K2GrabberController{

// public :

//     K2GrabberController(net::K2UdpReaderManager *udpReaderManager, std::vector<net::Interface> *localInterfaces, size_t id, tool::cam::K2GrabberTargetInfo info);

//     // init
//     void init_connections();

//     // threads
//     void quit_threads();

// // slots

//     // connections
//     bool connect_grabber();
//     void disconnect_grabber();

//     // commands
//     void ask_new_frame();
//     void open_camera_mode(cam::K2FrameRequest mode);
//     void close_camera_mode();
//     void set_camera_parameters(cam::K2Settings p);

//     // settings
//     void update_size_udp_packet(int size);

// // signals

//     sigslot::signal<std::string > stack_trace_message_signal;
//     sigslot::signal<size_t, bool > writing_connected_state_signal;
//     sigslot::signal<size_t, bool > reading_connected_state_signal;
//     sigslot::signal<size_t, size_t> packet_received_signal;
//     sigslot::signal<size_t, size_t, size_t, size_t> frame_dropped_signal;
//     sigslot::signal<size_t, size_t, std::int64_t, std::int64_t> frame_processed_signal;

//     sigslot::signal<size_t, cam::K2CloudDisplayData *> update_cloud_data_signal;
//     sigslot::signal<size_t, cam::K2MeshDisplayData *> update_mesh_data_signal;
//     sigslot::signal<size_t, cam::K2BodiesDisplayData *> update_bodies_data_signal;

// public:

//     // grabber id
//     size_t idC = 0;


// private:

//     // connections
//     bool m_wConnection = false;

//     // tcp sender
//     std::unique_ptr<net::K2ScanerTcpSender> m_tcpSender = nullptr;

//     // udp reader
//     net::K2UdpReaderManager *m_udpReaderManager = nullptr;

//     // data processing
//     std::unique_ptr<K2ProcessData> m_dataProcess = nullptr;
//     std::unique_ptr<std::thread> m_dataProcessT = nullptr;

//     // frame reader
//     std::unique_ptr<net::K2FrameReader> m_frameReader = nullptr;
//     std::unique_ptr<std::thread> m_frameReaderT;

//     std::int64_t m_lastTime = 0;
//     size_t m_packetsReceived = 0;
//     size_t m_framesReceived = 0;
//     size_t m_packetsLost = 0;
//     size_t m_framesLost = 0;

//     cam::K2GrabberTargetInfo m_info;
// };

// }

