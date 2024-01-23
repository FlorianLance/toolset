
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

// Qt
#include <QObject>

// local
#include "camera/impl/k2_manager.hpp"

namespace tool::scan{

class CameraWorker;
using CameraWorkerUP = std::unique_ptr<CameraWorker>;

class CameraWorker : public QObject{
    Q_OBJECT
public:

    CameraWorker() = default;

    bool is_initialized()const {return m_kinectManager.is_initialized();}
    cam::K2FrameRequest mode() const {return m_kinectManager.mode();}

public slots:

    void kill_loop();

    void open_camera(cam::K2FrameRequest mode);

    void manager_read_frame();

    void update_camera_parameters(cam::K2Settings parameters);
    void set_loop_state(bool state);
    void close_camera();

    void save_camera_intrisics(QString path);
    void save_camera_space_depth_table(QString path);
    void save_settings_config_file(QString path);

private :

    std::int64_t read_frame();

signals:

    void camera_opened_signal(bool state);
    void new_frame_signal(std::int64_t time, std::shared_ptr<cam::K2Frame> frame);
    void no_frame_signal();
    void loop_ended_signal();

public:

    std::atomic_bool isReading = false;

private:

    bool m_isLooping = false;
    int m_fps = 30;    
    cam::K2Manager m_kinectManager;
};
}
