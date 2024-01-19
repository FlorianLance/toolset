
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

#include "camera_worker.hpp"

// Qt
#include <QTime>
#include <QThread>
#include <QCoreApplication>
#include <QDebug>

// base
#include "utility/benchmark.hpp"
#include "camera/impl/k2_config_files.hpp"

// qt-utility
#include "qt_logger.hpp"

#include <iostream>

using namespace tool;
using namespace tool::scan;
using namespace tool::camera;


void CameraWorker::open_camera(K2FrameRequest mode){

    if(auto success = m_kinectManager.open_kinect(mode)){
        QtLogger::message(QSL("Kinect opened with mode ") % QString::number(static_cast<int>(mode)));
        emit camera_opened_signal(true);
    }else{
        QtLogger::error(QSL("Cannot open kinect with mode ") % QString::number(static_cast<int>(mode)));
        emit camera_opened_signal(false);
    }



    QThread::msleep(20);
}

std::int64_t CameraWorker::read_frame(){

    isReading = true;
    std::int64_t timeGetFrameMicroS = m_kinectManager.get_data();
    isReading = false;

    if(timeGetFrameMicroS >= 0){
        emit new_frame_signal(timeGetFrameMicroS, m_kinectManager.frame);
    }else{
        emit no_frame_signal();
    }

    return timeGetFrameMicroS;
}

void CameraWorker::manager_read_frame(){
    kill_loop();
    read_frame();
}

void CameraWorker::update_camera_parameters(K2Settings parameters){
    m_fps = parameters.fps;
    if(m_fps == 0){
        m_fps = 30;
    }
    m_kinectManager.update_parameters(std::move(parameters));
}

void CameraWorker::close_camera(){
    m_kinectManager.close_kinect();
    emit camera_opened_signal(false);
}

void CameraWorker::save_camera_intrisics(QString path){
    m_kinectManager.kinect.save_camera_intrisics(path.toStdString());
}

void CameraWorker::save_camera_space_depth_table(QString path){
    m_kinectManager.kinect.save_camera_space_depth_table(path.toStdString());
}

void CameraWorker::save_settings_config_file(QString path){
    K2ConfigFiles::save_grabber_settings_config_file(m_kinectManager.kinect.parameters, path.toStdString());
}

void CameraWorker::kill_loop(){
    m_isLooping = false;
}

void CameraWorker::set_loop_state(bool state){

    m_isLooping = state;
    while(m_isLooping){

        Bench::reset();

        // read frame
        Bench::start("CameraWorker::read_frame");
        std::int64_t timeGetFrameMicroS = read_frame();
        Bench::stop();


        Bench::start("CameraWorker::wait");
        QTime dieTime;
        if(timeGetFrameMicroS >= 0){

            // valid frame
            std::int64_t timeGetFrameMs = timeGetFrameMicroS/1000;
            std::int64_t msPerFrame = 1000/m_fps;
            bool addFspTime = timeGetFrameMs < msPerFrame;
            dieTime = QTime::currentTime().addMSecs(5 + (addFspTime ? static_cast<int>(msPerFrame-timeGetFrameMs) : 0));
        }else{
            // no frame
            dieTime = QTime::currentTime().addMSecs(5);
        }

        while( QTime::currentTime() < dieTime){
            QCoreApplication::processEvents( QEventLoop::AllEvents, 5);
        }

        Bench::stop();
//        Bench::display();

    }
    emit loop_ended_signal();
}


#include "moc_camera_worker.cpp"
