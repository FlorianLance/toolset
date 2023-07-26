
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
#include <QImage>

// local
#include "camera/kinect2/k2_types.hpp"

namespace tool::ui{

struct DisplayTimes{
    qint64 copy=0, color=0, depth=0, infra=0, cloud=0;
};

class ProcessDataWorker;
using ProcessDataWorkerUP = std::unique_ptr<ProcessDataWorker>;

class ProcessDataWorker : public QObject{
    Q_OBJECT

public :

    ProcessDataWorker();
    ~ProcessDataWorker();

public slots:

    void process_and_display(std::shared_ptr<camera::K2Frame> frame);
    void save(const std::string &path);

signals:

    // display update signals
    //  # time
    void process_time_signal(qint64);
    // # images
    void update_color_signal(QImage);
    void update_depth_signal(QImage);
    void update_infra_signal(QImage);
    // # opengl
    void update_cloud_data_signal(camera::K2CloudDisplayData *cloudData);
    void update_mesh_data_signal(camera::K2MeshDisplayData *meshData);
    void update_joint_data_signal(std::array<camera::K2BodyInfos,6> *bodiesData);

private :

    void process_compress_color_1920x1080();
    void process_compress_color_512x424();
    void process_compress_color_cloud();
    void process_compress_color_mesh();
    void process_depth_512x424();
    void process_infra_512x424();

public :

    std::atomic_bool isProcessing = false;

private :

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}
