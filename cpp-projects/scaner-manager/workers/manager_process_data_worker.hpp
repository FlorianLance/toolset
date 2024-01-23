
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
#include <QObject>
#include <QElapsedTimer>

// base
#include "camera/impl/k2_network.hpp"

// qt_utility
#include "gui/widgets/image_viewer.hpp"

namespace tool::ui{

class ProcessDataWorker;
using ProcessDataWorkerUP = std::unique_ptr<ProcessDataWorker>;

class ProcessDataWorker : public QObject{
    Q_OBJECT

public :

    ProcessDataWorker(size_t id);
    ~ProcessDataWorker();

    std::shared_ptr<cam::K2Frame> processed_frame();

    cam::K2CloudDisplayData *cloud_data();
    cam::K2MeshDisplayData *mesh_data();

public slots:

    void process_data_from_udp_frame(cam::K2UdpHeader header, std::shared_ptr<cam::K2Frame> frame);

signals:

    void end_update_image_signal(std::int64_t);

    // # images
    void update_color_signal(QImage);
    void update_depth_signal(QImage);
    void update_infra_signal(QImage);
    // # opengl
    void update_cloud_data_signal(cam::K2CloudDisplayData *coloredCloudData);
    void update_mesh_data_signal(cam::K2MeshDisplayData *coloredMeshData);
    void update_bodies_data_signal(std::array<cam::K2BodyInfos,6> *bodiesData);

    // recording
    void new_frame_to_save_signal(std::shared_ptr<cam::K2Frame> frame);

private :

    void process_depth();
    void process_infra();
    void process_compressed_color_1920x1080();
    void process_compressed_color_512x424();
    void process_compressed_color_cloud();
    void process_compressed_color_mesh();

public :

    std::atomic_bool isProcessing = false;
    std::atomic_bool videoSavingState = false;

private :


    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}
