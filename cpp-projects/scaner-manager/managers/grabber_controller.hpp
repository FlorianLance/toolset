
/*******************************************************************************
** Toolset-scaner-manager                                                     **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#pragma once

// Qt
#include <QThread>
#include <QMatrix4x4>
#include <QTimer>

// qt_utility
#include "gui/widgets/grabber_parameters_widget.hpp"

// base
#include "utility/vector.hpp"
#include "network/network_enums.hpp"
#include "camera/impl/k2_config_files.hpp"

// local
#include "managers/udp_reader_manager.hpp"
#include "workers/tcp_sender_worker.hpp"
#include "workers/manager_process_data_worker.hpp"
#include "workers/frame_reader_worker.hpp"

namespace tool::ui{


class GrabberController;
using GrabberControllerUP = std::unique_ptr<GrabberController>;

class GrabberController : public QObject{

    Q_OBJECT
public:

    GrabberController(network::K2UdpReaderManager *udpReaderManager, std::vector<network::Interface> *localInterfaces, size_t id, camera::K2GrabberTargetInfo info, QColor color);

    // init
    void init_connections();

    // threads
    void quit_threads();
    void wait_for_threads();

    void set_video_saving_state(bool state);

public slots:

    // connections
    void connect_grabber();
    void disconnect_grabber();

    // commands
    void ask_new_frame();
    void open_camera_mode(camera::K2FrameRequest mode);
    void close_camera_mode();
    void shutdown_computer();
    void restart_computer();
    void ask_to_save_settings_file();

    // settings
    void send_ui_settings(camera::K2Settings settings);
    void send_current_ui_settings();
    void update_current_ui_settings(camera::K2Settings settings);
    void copy_current_ui_settings();

    // display options
    void force_color_cloud(bool state);
    void display_cloud();
    void hide_cloud();
    void set_size_cloud(qreal size);
    void update_model_matrix(const geo::Mat4<double> &m);
    geo::Mat4<double> get_model_matrix() const;
    void send_current_ui_display_options();

    // calibration
    void register_cloud(camera::K2CloudDisplayData *cloudData);
    void process_registered_clouds();

    // IO
    void save_current_cloud(QString path);
    void save_current_mesh(QString path);

    camera::K2CloudDisplayData *cloud_data();
    camera::K2MeshDisplayData *mesh_data();

signals:

    void writing_connected_state_signal(bool);

    // udp
    void all_packets_received_signal(std::vector<QByteArray*> data);
    void update_size_udp_packet_signal(int size);

    // # commands
    void open_camera_command_signal(camera::K2FrameRequest mode);
    void ask_new_frame_command_signal();
    void shutdown_computer_signal();
    void restart_computer_signal();
    void ask_to_save_settings_file_signal();

    // settings    
    void update_ui_with_settings_signal(camera::K2Settings settings);
    void copy_settings_parameters_signal(camera::K2Settings settings);
    // display
    void update_display_parameters_signal(size_t id, DisplayOptions p);

    // 3D
    void process_frame_signal(camera::K2UdpHeader header, std::shared_ptr<camera::K2Frame> data);
    void update_registers_clouds_data_signal(size_t id, std::vector<geo::Pt3f> *calibCloudData, std::vector<geo::Pt3f> *calibColorData);
    void update_cloud_data_signal(size_t id, camera::K2CloudDisplayData *cloudData);
    void update_mesh_data_signal(size_t id, camera::K2MeshDisplayData *meshData);
    void update_bodies_data_signal(size_t id, std::array<camera::K2BodyInfos,6> *bodiesData);

    // misc
    void kill_workers_signal();
    void timeout_signal();

    void new_frame_to_save_signal(size_t camId, std::shared_ptr<camera::K2Frame> frame);

public:

    // camera id
    size_t idC = 0;
    QColor initColorC   = Qt::red;
    QColor invalidColorC = Qt::black;

    // parameters
    // # calibration
    std::atomic_bool resetCalibCloud = false;
    std::atomic_bool showCalibration = false;
    std::atomic_bool useProcessed = false;
    std::atomic_bool registerOneCloud = false;

    std::atomic_bool computeSphereCenter = true;
    std::atomic<float> offsetDepth = 0.02f;
    std::atomic<double> maxDistanceCorr = 0.075;
    std::atomic<float> maxDistanceOutlier = 0.05f;
    std::atomic<bool> removeOutliers = false;

//    std::atomic<float> factorCancel = 0.5f;


    // calibration
    std_v2<geo::Pt3f> registeredPoints;
    std_v2<geo::Pt3f> registeredColors;   

    std::vector<geo::Pt3f> processedPoints;
    std::vector<geo::Pt3f> processedColors;
    std::vector<geo::Pt3f> flattenRegisteredPoints;
    std::vector<geo::Pt3f> flattenRegisteredColors;


    // timers
    QTimer registerCloudT;

    // ui
    GrabberParametersW grabberParametersW;
    ImageViewer imageViewer;

    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimeReceived = std::chrono::high_resolution_clock::now();

    // connections
    bool m_wConnection = false;

private:

    // workers
    std::unique_ptr<network::FrameReaderW> m_frameReaderW = nullptr;
    std::unique_ptr<network::TcpSenderWorker> m_tcpSenderW = nullptr;
    ProcessDataWorkerUP m_processDataW = nullptr;

    // manager
    network::K2UdpReaderManager *m_udpReaderManager = nullptr;

    // threads
    QThread m_tcpSenderT;
    QThread m_dataT;
    QThread m_frameReaderT;

    int m_framesReceived = 0;
    int m_framesLost = 0;
    int m_packetsReceived = 0;
    int m_packetsLost = 0;

};
}
