
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

#include "grabber_controller.hpp"

// std
#include <fstream>
#include <iostream>

// Qt
#include <QNetworkInterface>
#include <QFileDialog>

// base
#include "utility/types.hpp"

// qt_utility
#include "qt_str.hpp"

using namespace std::chrono;
using namespace tool;
using namespace tool::ui;
using namespace tool::network;
using namespace tool::camera;
using namespace tool::geo;

GrabberController::GrabberController(UdpReaderManager *udpReaderManager, std::vector<Interface> *localInterfaces, size_t id, K2GrabberTargetInfo info, QColor color) : idC(id), initColorC(color){

    // init workers
    m_udpReaderManager  = udpReaderManager;
    m_tcpSenderW        = std::make_unique<TcpSenderWorker>(id, localInterfaces);
    m_processDataW      = std::make_unique<ProcessDataWorker>(id);
    m_frameReaderW      = std::make_unique<FrameReader>();

    // move threads
    m_tcpSenderW->moveToThread(&m_tcpSenderT);
    m_processDataW->moveToThread(&m_dataT);
    m_frameReaderW->moveToThread(&m_frameReaderT);

    // set workers/threads names
    m_tcpSenderW->setObjectName("tcp_worker_" + QString::number(idC));

    m_processDataW->setObjectName("image_worker_" + QString::number(idC));
    m_tcpSenderT.setObjectName("tcp_thread_" + QString::number(idC));

    m_dataT.setObjectName("image_thread_" + QString::number(idC));

    m_frameReaderT.setObjectName("fr_" + QString::number(idC));

    // init ui    
    grabberParametersW.init_from_manager(color, localInterfaces, info);

    // start threads
    m_tcpSenderT.start();
    m_dataT.start();
    m_frameReaderT.start();

    // set connections
    init_connections();
}

void GrabberController::init_connections(){

    using GM  = GrabberController;
    using GPW = GrabberParametersW;
    using TCP = TcpSenderWorker;
    using UDP = UdpReaderManager;
    using FR  = FrameReader;
    using PD  = ProcessDataWorker;
    auto tcpW   = m_tcpSenderW.get();
    auto frW    = m_frameReaderW.get();
    auto udpMW  = m_udpReaderManager;
    auto pdW    = m_processDataW.get();
    auto spW    = &grabberParametersW;

    // from this
    // -> tcp
    connect(this, &GM::ask_new_frame_command_signal, tcpW, &TCP::ask_new_frame_command);
    connect(this, &GM::shutdown_computer_signal, tcpW, &TCP::shutdown_computer);
    connect(this, &GM::restart_computer_signal, tcpW, &TCP::restart_computer);
    connect(this, &GM::kill_workers_signal,          tcpW, &TCP::disable_writing);
    connect(this, &GM::update_ui_with_settings_signal, tcpW, &TCP::update_settings_command);
    connect(this, &GM::ask_to_save_settings_file_signal, tcpW, &TCP::ask_to_save_settings_file_command);

    // -> frame reader
    connect(this, &GM::all_packets_received_signal,      frW, &FR::process_packets);
    // -> data process
    connect(this, &GM::process_frame_signal, pdW, &PD::process_data_from_udp_frame);
    // ->udp
    connect(this, &GM::update_size_udp_packet_signal, tcpW, &TCP::set_size_udp_packets);


    // from grabber parameters ui
    // -> tcp
    connect(spW, &GPW::send_manager_writing_connection_parameters_signal,  tcpW, &TCP::enable_writing);
    connect(spW, &GPW::disable_connection_signal,                         tcpW, &TCP::disable_writing);
    connect(spW, &GPW::open_camera_signal,                                tcpW, &TCP::open_camera_command);
    connect(spW, &GPW::close_camera_signal,                               tcpW, &TCP::close_camera_command);
    connect(spW, &GPW::send_settings_parameters_signal,                     tcpW, &TCP::update_settings_command);
    // -> this
    connect(spW, &GPW::copy_camera_parameters_signal,  this, &GM::copy_settings_parameters_signal);
    connect(spW, &GPW::send_display_options_signal, this, [&](DisplayOptions p){
        emit update_display_parameters_signal(idC, p);
    });


    // from tcp    
    // # -> this
    connect(tcpW, &TCP::connection_state_signal,    this, [&](bool state){
        grabberParametersW.update_writing_connection_state(state);
        emit writing_connected_state_signal(m_wConnection = state);
    });
    // -> ui
    connect(tcpW, &TCP::message_signal,  &grabberParametersW, &GPW::add_network_log_message);


    // from frame reader    
    // -> display
    connect(frW, &FR::frame_received_signal, this, [&](K2UdpHeader header, std::shared_ptr<K2Frame> frame){
        if(!m_processDataW->isProcessing){
            const auto timeFrame = duration_cast<microseconds>(nanoseconds(high_resolution_clock::now().time_since_epoch())- nanoseconds(frame->timeStampGetFrame));
            grabberParametersW.update_last_frame_time(timeFrame.count());
            emit process_frame_signal(header, frame);
        }
    });


    // from udp manager    
    connect(udpMW, &UDP::packet_received_signal, this, [=](size_t idInterface, int idGrabber, size_t){
        if(idInterface == to_size_t(tcpW->reading_interface()) && idGrabber == to_int(idC)){
            ++m_packetsReceived;
            grabberParametersW.update_frames_info(m_packetsReceived, m_packetsLost, m_framesReceived, m_framesLost);
        }
    });
    connect(udpMW, &UDP::frame_dropped_signal, this, [=](size_t idInterface, int idGrabber, size_t, size_t packetsDropped){
        if(idInterface == to_size_t(tcpW->reading_interface()) && idGrabber == to_int(idC)){
            ++m_framesLost;
            m_packetsLost += packetsDropped;
            grabberParametersW.update_frames_info(m_packetsReceived, m_packetsLost, m_framesReceived, m_framesLost);
        }
    });
    connect(udpMW, &UDP::all_packets_received_signal, this, [=](size_t idInterface, int idGrabber, std::vector<QByteArray*>packets){
        if(to_int(idInterface) == tcpW->reading_interface() && idGrabber == to_int(idC)){
            ++m_framesReceived;
            lastTimeReceived = std::chrono::high_resolution_clock::now();
            emit all_packets_received_signal(packets);
            grabberParametersW.update_frames_info(m_packetsReceived, m_packetsLost, m_framesReceived, m_framesLost);
        }
    });

    // from process data
    // -> this
    connect(pdW, &PD::update_cloud_data_signal, this, [&](camera::K2CloudDisplayData *cloudData){

        // register cloud
        if(registerCloudT.remainingTime() > 0){

            // add current cloud data to registered clouds
            register_cloud(cloudData);

            if(registerOneCloud){ // stop timer for registering only one cloud
                registerCloudT.stop();
            }
        }

        if(!showCalibration){ // display current cloud
            emit update_cloud_data_signal(idC, cloudData);
        }else{
            if(useProcessed ){ // display processed registered clouds
                emit update_registers_clouds_data_signal(idC, &processedPoints, &processedColors);
            }else{  // display raw registered clouds
                emit update_registers_clouds_data_signal(idC, &flattenRegisteredPoints, &flattenRegisteredColors);
            }
        }
    });
    connect(pdW, &PD::update_mesh_data_signal,  this, [&](K2MeshDisplayData *meshData){
        emit update_mesh_data_signal(idC, meshData);
    });
    connect(pdW, &PD::update_bodies_data_signal, this, [&](std::array<K2BodyInfos,6> *bodiesData){
        emit update_bodies_data_signal(idC, bodiesData);
    });
    connect(pdW, &PD::new_frame_to_save_signal, this, [&](std::shared_ptr<K2Frame> frame){
        emit new_frame_to_save_signal(idC, frame);
    });

    // -> ui
    connect(pdW, &PD::update_color_signal, &imageViewer, &ImageViewer::set_image);
    connect(pdW, &PD::update_depth_signal, &imageViewer, &ImageViewer::set_image);
    connect(pdW, &PD::update_infra_signal, &imageViewer, &ImageViewer::set_image);


}

void GrabberController::quit_threads(){

    emit kill_workers_signal();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    m_tcpSenderT.quit();
    m_dataT.quit();
    m_frameReaderT.quit();
}

void GrabberController::wait_for_threads(){
    m_tcpSenderT.wait();
    m_dataT.wait();
    m_frameReaderT.wait();
}

void GrabberController::set_video_saving_state(bool state){
    m_processDataW->videoSavingState = state;
}

void GrabberController::connect_grabber(){
    grabberParametersW.send_writing_connection_parameters();
    grabberParametersW.send_reading_connection_parameters();
}

void GrabberController::disconnect_grabber(){
    emit grabberParametersW.disable_connection_signal();
}

void GrabberController::open_camera_mode(K2FrameRequest cameraMode){
    grabberParametersW.open_camera(cameraMode);
}

void GrabberController::close_camera_mode(){
    emit grabberParametersW.close_camera_signal();
}

void GrabberController::force_color_cloud(bool state){
    grabberParametersW.force_color_cloud(state);
    send_current_ui_display_options();
}

void GrabberController::display_cloud(){
    grabberParametersW.display_cloud();
    send_current_ui_display_options();
}

void GrabberController::hide_cloud(){
    grabberParametersW.hide_cloud();
    send_current_ui_display_options();
}

void GrabberController::set_size_cloud(qreal size){
    grabberParametersW.set_size_cloud(size);
    send_current_ui_display_options();
}

void GrabberController::send_current_ui_settings(){
    grabberParametersW.send_current_ui_settings();
}

void GrabberController::send_ui_settings(camera::K2Settings settings){
    grabberParametersW.send_ui_settings(settings);
}

void GrabberController::send_current_ui_display_options(){
    grabberParametersW.send_current_ui_display_options();
}

void GrabberController::update_current_ui_settings(camera::K2Settings settings){
    grabberParametersW.update_ui_settings(settings);
}

void GrabberController::copy_current_ui_settings(){
    grabberParametersW.copy_current_ui_settings();
}

void GrabberController::update_model_matrix(const Mat4<double> &m){
    grabberParametersW.update_transformation(m);
    grabberParametersW.send_current_ui_display_options();
}

Mat4<double> GrabberController::get_model_matrix() const{
    return grabberParametersW.get_transformation();
}

void GrabberController::register_cloud(K2CloudDisplayData *cloudData){

    // reset cloud
    if(resetCalibCloud){
        registeredPoints.clear();
        registeredColors.clear();
        flattenRegisteredPoints.clear();
        flattenRegisteredColors.clear();
        resetCalibCloud = false;
    }

    // fill clouds
    const auto validColor = geo::Pt3f{initColorC.red()/255.f,initColorC.green()/255.f,initColorC.blue()/255.f};
    std::vector<Pt3f> points;
    std::vector<Pt3f> colors;
    points.reserve(cloudData->sizePts);
    colors.reserve(cloudData->sizePts);
    flattenRegisteredPoints.reserve(flattenRegisteredPoints.size() + cloudData->sizePts);
    flattenRegisteredColors.reserve(flattenRegisteredColors.size() + cloudData->sizePts);

    for(size_t ii = 0; ii < cloudData->sizePts; ++ii){
        points.emplace_back((*cloudData->points)[ii]);
        colors.emplace_back(validColor);
        flattenRegisteredPoints.emplace_back((*cloudData->points)[ii]);
        flattenRegisteredColors.emplace_back(validColor);
    }

    registeredPoints.emplace_back(std::move(points));
    registeredColors.emplace_back(std::move(colors));
}

void GrabberController::process_registered_clouds(){

    const auto validColor   = geo::Pt3f{initColorC.red()/255.f,initColorC.green()/255.f,initColorC.blue()/255.f};

    // clean processed data
    processedPoints.clear();
    processedColors.clear();

    // outlier for each cloud
    auto squareMaxDist = static_cast<double>(maxDistanceOutlier * maxDistanceOutlier);

    // compute means for removing outliers
    std::vector<Pt3f> means;
    for(const auto &reg : registeredPoints){
        Pt3f mean{0,0,0};
        for(const auto &pt : reg){
            mean += pt;// + Pt3f{0,0,offsetDepth};
        }
        means.emplace_back(mean/(1.f*reg.size()));
    }

    // remove outliers
    if(computeSphereCenter){ // keep only one mean point for each registered cloud

        double minRay = 0.;
        for(size_t idCloud = 0; idCloud < registeredPoints.size(); ++idCloud){

            const auto cloudMean = means[idCloud].conv<double>();
            std::vector<Pt3d> cloudPointsWithoutOutliers;
            Pt3d mean{0,0,0};
            // remove outliers
            for(const auto &pt : registeredPoints[idCloud]){
                const auto np = pt.conv<double>();
                if((square_norm(Vec3d{vec(np,cloudMean)}) < squareMaxDist) || !removeOutliers){ // check if current point not an outlier
                    mean += np; // add to the mean computing
                    cloudPointsWithoutOutliers.emplace_back(np);
                }
            }
            const size_t n = cloudPointsWithoutOutliers.size();
            if(n > 0){
                mean = mean/ (1.0*n);
            }else{
                continue;
            }

            // compute A
            Mat3d A;
            for(size_t ii = 0; ii < cloudPointsWithoutOutliers.size(); ++ii){
                const auto &p = cloudPointsWithoutOutliers[ii];
                A.array[0] += (p.x() * (p.x() - mean.x()))/n;
                A.array[1] += (p.x() * (p.y() - mean.y()))/n;
                A.array[2] += (p.x() * (p.z() - mean.z()))/n;
                A.array[3] += (p.y() * (p.x() - mean.x()))/n;
                A.array[4] += (p.y() * (p.y() - mean.y()))/n;
                A.array[5] += (p.y() * (p.z() - mean.z()))/n;
                A.array[6] += (p.z() * (p.x() - mean.x()))/n;
                A.array[7] += (p.z() * (p.y() - mean.y()))/n;
                A.array[8] += (p.z() * (p.z() - mean.z()))/n;
            }
            A *= 2.;

            // compute B
            Vec3d B;
            for(size_t ii = 0; ii < n; ++ii){
                const auto &p = cloudPointsWithoutOutliers[ii];
                const auto xx = p.x()*p.x();
                const auto yy = p.y()*p.y();
                const auto zz = p.z()*p.z();
                const auto t = (xx + yy + zz);
                B.x() += (t * (p.x() - mean.x()))/n;
                B.y() += (t * (p.y() - mean.y()))/n;
                B.z() += (t * (p.z() - mean.z()))/n;
            }

            Mat3d bb = inverse(transpose(A)*A) * transpose(A);
            Pt3d center =  B * bb;//bb * B;
            double r = 0.;
            for(size_t ii = 0; ii < n; ++ii){
                const auto &p = cloudPointsWithoutOutliers[ii];
                r +=    (p.x()-center.x())*(p.x()-center.x()) +
                        (p.y()-center.y())*(p.y()-center.y()) +
                        (p.z()-center.z())*(p.z()-center.z());
            }
            r = sqrt(r/n);
            minRay += r;

            processedPoints.emplace_back(center.conv<float>());
            processedColors.emplace_back(validColor);
        }

        if(registeredPoints.size() > 0){
            minRay = minRay/registeredPoints.size();
        }

    }else{ // use all points from registered cloud

        for(size_t idCloud = 0; idCloud < registeredPoints.size(); ++idCloud){
            const auto cloudMean = means[idCloud];
            for(const auto &pt : registeredPoints[idCloud]){
                const auto np = pt + Pt3f{0,0,offsetDepth};
                if((static_cast<double>(square_norm(Vec3f{vec(np,cloudMean)})) < squareMaxDist) || !removeOutliers){ // check if current point not an outlier
                    processedPoints.emplace_back(np);
                    processedColors.emplace_back(validColor);
                }
            }
        }
    }

}

void GrabberController::save_current_cloud(QString path){

    auto cloud = cloud_data();
    cloud->dataLocker.lock();

    if(path.length() == 0){
        path = QFileDialog::getSaveFileName(nullptr, "Select cloud file", "", "OBJ file (*.obj)");
    }

    std::ofstream file;
    std::string name(path.toStdString());
    file.open(name);
    if(file.is_open()){
        for(size_t ii = 0; ii < cloud->sizePts; ++ii){
            const auto &v = (*cloud->points)[ii];
            const auto &c = (*cloud->colors)[ii];
            file << "v " << v.x() << " " << v.y() << " " << v.z() << " " << c.x() << " " << c.y() << " " << c.z() << "\n";
        }

        for(size_t ii = 0; ii < cloud->sizePts; ++ii){
            const auto &n = (*cloud->normals)[ii];
            file << "vn " << n.x() << " " << n.y() << " " << n.z() << "\n";
        }
    }
    file.close();
    cloud->dataLocker.unlock();
}

void GrabberController::save_current_mesh(QString path){

    auto mesh = mesh_data();
    mesh->dataLocker.lock();

    if(path.length() == 0){
        path = QFileDialog::getSaveFileName(nullptr, "Select cloud file", "", "OBJ file (*.obj)");
    }

    std::ofstream file;
    std::string name(path.toStdString());
    file.open(name);
    if(file.is_open()){
        for(size_t ii = 0; ii < mesh->sizePts; ++ii){
            const auto &v = (*mesh->points)[ii];
            const auto &c = (*mesh->colors)[ii];
            file << "v " << v.x() << " " << v.y() << " " << v.z() << " " << c.x() << " " << c.y() << " " << c.z() << "\n";
        }

        for(size_t ii = 0; ii < mesh->sizePts; ++ii){
            const auto &n = (*mesh->normals)[ii];
            file << "vn " << n.x() << " " << n.y() << " " << n.z() << "\n";
        }

        for(size_t ii = 0; ii < mesh->sizeTris; ++ii){
            const auto &t = (*mesh->triangles)[ii];
            file << "f " << (t.x()+1) << " " << (t.y()+1) << " " << (t.z()+1) << "\n";
        }
    }
    file.close();
    mesh->dataLocker.unlock();
}

K2CloudDisplayData *GrabberController::cloud_data(){
    return m_processDataW->cloud_data();
}

K2MeshDisplayData *GrabberController::mesh_data(){
    return m_processDataW->mesh_data();
}

void GrabberController::ask_new_frame(){
    emit ask_new_frame_command_signal();
}

void GrabberController::shutdown_computer(){
    emit shutdown_computer_signal();
}

void GrabberController::restart_computer(){
    emit restart_computer_signal();
}

void GrabberController::ask_to_save_settings_file(){
    emit ask_to_save_settings_file_signal();
}



#include "moc_grabber_controller.cpp"


//void GrabberController::update_size_udp_packet(int size){
//    m_tcpSenderW->set_size_udp_packets(size); // TODO
//}

//void GrabberController::update_ui_with_camera_parameters(Kinect2Parameters parameters){
//    m_tcpSenderW->update_settings_command(parameters);
//}

//void GrabberController::update_ui_with_display_parameters(DisplayParameters parameters){
//    Q_UNUSED(parameters)
//    // ...
//}

//void GrabberController::set_calibration_settings(){
//    grabberParametersW.set_calibration_parameters();
//}

//void GrabberController::set_default_settings(){
//    grabberParametersW.set_default_parameters();
//}
