
/*******************************************************************************
** Toolset-scaner-grabber                                                     **
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

// std
#include <memory>
#include <utility>
#include <queue>

// Qt
#include <QMainWindow>
#include <QHostAddress>
#include <QThread>

// qt-utility
#include "gui/widgets/grabber_parameters_widget.hpp"
#include "gui/widgets/image_viewer.hpp"

// local
#include "camera/impl/k2_manager.hpp"
// # ui
#include "ui_GrabberW.h"
// # network
#include "workers/tcp_reader_worker.hpp"
#include "workers/udp_sender_worker.hpp"
// # data
#include "workers/camera_worker.hpp"
// # display
#include "workers/grabber_process_data_worker.hpp"
// # widgets
#include "widgets/depth_camera_gl_widget.hpp"


// ui
namespace Ui {
    class GrabberW;
}



namespace tool::ui{


class ScanerGrabberWindow : public QMainWindow{

    Q_OBJECT
public:

    ScanerGrabberWindow();
    ~ScanerGrabberWindow();

private slots:

    void new_frame(std::int64_t time, std::shared_ptr<camera::K2Frame> frame);

    void save_cloud();
    void start_saving_cloud();
    void stop_saving_cloud();    

    void update_display_state(bool state);
    void set_display_parameters(DisplayOptions parameters);

    void read_tcp_command(camera::K2TcpPacket command);
    void new_tcp_connection_signal(QString address, QString port);
    void tcp_connection_ended(QString address, QString port);
    void update_tcp_connection_state(QString readingAddress, int readingPort, bool state);

    void update_udp_connection_state(QString writingAddress, int writingPort, bool state);
    void update_udp_packet_failure(size_t count);
    void update_udp_frame_sent(std::int64_t time);
    void update_udp_nb_bytes_sent(qint64 timeStamp, size_t nbBytesSent);

    void update_cloud_data(camera::K2CloudDisplayData *cloudData);
    void update_mesh_data(camera::K2MeshDisplayData *meshData);

    void set_process_time(std::int64_t time);
    void erase_time_get_data_timeout();
    void erase_time_display_timeout();
    void erase_time_send_data_timeout();
    void erase_time_send_rate_timeout();

    void save_camera_intrinsics();
    void save_camera_depth_space_table();
    void save_settings_config_file();
    void load_settings_config_file();

private:

    void set_meta_types();
    void init_connections();

signals:

    // loops
    void start_camera_loop_signal();
    void kill_camera_loop_signal();

    // frame
    void send_frame_signal(camera::K2TcpPacket, std::shared_ptr<camera::K2Frame>);
    void process_and_display_signal(std::shared_ptr<camera::K2Frame>);

    // camera
    void save_camera_intrinsics_signal(QString path);
    void save_camera_space_depth_table_signal(QString path);
    void save_settings_config_file_signal(QString path);

    void read_new_frame_signal();

private:

    Ui::GrabberW *m_ui = nullptr;
    GrabberParametersW m_pW;

    // widgets
    bool m_displayData = true;
    ImageViewer m_colorW;
    ImageViewer m_depthW;
    ImageViewer m_infraW;
    DepthCameraGlWidget m_depthCameraW;

    // network
    // # tcp
    network::TcpReaderUP m_tcpReader = nullptr;
    // # udp
    QThread m_udpSenderT;
    network::UdpSenderWorkerUP m_udpSenderW = nullptr;
    // data
    QThread m_dataT;
    ProcessDataWorkerUP m_dataW = nullptr;
    // camera
    QThread m_cameraT;
    scan::CameraWorkerUP m_cameraW = nullptr;

    // gl display parameters
    DisplayOptions m_displayParameters;

    // network
    int m_nbAskFrameReceived = 0;
    int m_nbFramesSent =0;
    int m_totalPacketFailure = 0;
    bool m_sendNextFrame = false;
    camera::K2TcpPacket m_lastCommand;

    // info
    std::vector<std::pair<qint64, size_t>> m_sendRate;

    // saved frames
    bool saveData = false;
    std::vector<std::shared_ptr<camera::K2Frame>> m_savedFrames;
    std::shared_ptr<camera::K2Frame> m_lastFrame = nullptr;

    // timers
    QTimer m_eraseTimeGetDataT;
    QTimer m_eraseTimeDisplayDataT;
    QTimer m_eraseTimeSendataT;
    QTimer m_eraseSendRateT;
};
}
