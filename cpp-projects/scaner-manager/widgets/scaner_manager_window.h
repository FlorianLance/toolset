
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
#include <QMainWindow>
#include <QTimer>

// qt_utility
#include "widgets/multi_depth_camera_gl_widget.hpp"

// local
#include "managers/grabber_controller.hpp"
#include "managers/calibration_manager.hpp"

namespace Ui {
class ManagerW;
}

namespace tool::ui{

class ScanerManagerWindow : public QMainWindow{

    Q_OBJECT

public:

    ScanerManagerWindow(net::Protocol interfaceProtocol);
    ~ScanerManagerWindow();

private:

    void init_managers(const std::vector<cam::K2GrabberTargetInfo> &infos);
    void init_ui(const std::vector<cam::K2GrabberTargetInfo> &infos);
    void init_connections();

public slots:

    void set_loop_ask_frames_state(bool state);
    void copy_camera_parameters(cam::K2Settings p);

    // threads
    void quit_threads();
    void wait_for_threads();

    // connections
    void connect_grabbers();
    void disconnect_grabbers();

    void shutdown_computers();
    void restart_computers();

    // tcp
    void open_cameras();
    void close_cameras();

    // cameras
    void focus_camera(int index);
//    void update_all_grabbers_with_identical_settings(cam::K2Settings parameters);
    void save_grabbers_settings_config_files();
    void save_video_file();

    // calibration
    void save_calibration();
    void load_calibration_config_file();
    void calibrate();
    void register_clouds();


    void send_all_settings();

signals:

//    void connect_all_grabbers_signal();
//    void disconnect_all_grabbers_signal();
    void set_loop_ask_frames_state_signal(bool);

private:

    // ui
    Ui::ManagerW *m_ui = nullptr;

    // widgets
    // # parameters
    GrabberParametersW m_allCamerasW;
    GrabberParametersW m_allCamerasCalibW;
    // # opengl
    MultiDepthCameraGlWidget *m_multiDepthCameraW = nullptr;
    // # images
    ImageViewer *m_currentImageV = nullptr;

    // managers
    std::unique_ptr<CalibrationManager> m_calibrationManager = nullptr;
    std::unique_ptr<net::K2UdpReaderManager> m_udpReaderManager = nullptr;
    std::vector<std::unique_ptr<GrabberController>> m_grabbersManager;

    // timers
    int m_totalTimeout = 0;
    QTimer m_askFrameTimer;

    std::vector<net::Interface> localInterfaces;

    bool saveVideo = false;
    std::vector<std::tuple<size_t, std::shared_ptr<cam::K2Frame>>> m_savedData;

};
}
