
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

#include "scaner_grabber_window.hpp"

// std
#include <fstream>
#include <filesystem>

// Qt
#include <QSpinBox>
#include <QDateTime>
#include <QFileDialog>
#include <QScrollBar>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QSplashScreen>
#include <QMetaMethod>
#include <QMessageBox>
#include <QProcess>

// base
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"
#include "utility/types.hpp"

// qt-utility
#include "qt_str.hpp"
#include "qt_logger.hpp"

// ui
#include "ui_about_dialog.h"

using namespace tool::ui;
using namespace tool::cam;
using namespace tool::scan;
using namespace tool::net;


int main(int argc, char *argv[]){



    QApplication a(argc, argv);
    QLocale::setDefault(QLocale::c());

    QPixmap pixmap(":/splash/ex_vr_splash");
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    splash.show();
    QTimer::singleShot(2000, &splash, &QWidget::close); // keep displayed for 2 seconds

    tool::ui::ScanerGrabberWindow w;
    w.show();

    return a.exec();
}

ScanerGrabberWindow::ScanerGrabberWindow(){

    tool::Bench::reset();

    //     // init ui
    m_ui = new Ui::GrabberW();
    m_ui->setupUi(this);
    m_ui->twKinectData->setCurrentIndex(3);
    setWindowTitle("Scanner grabber");

    // connections
    set_meta_types();

    // init loggers
    Logger::no_file_init();
    QtLogger::init(
        QString(QApplication::applicationDirPath() % QSL("/logs")),
        QString(QSL("grabber_") % QHostInfo::localHostName() % QSL(".html"))
    );
    Logger::get()->message_signal.connect([&](std::string message){
        QtLogger::get()->message(QString::fromStdString(message));
    });
    Logger::get()->warning_signal.connect([&](std::string message){
        QtLogger::get()->warning(QString::fromStdString(message));
    });
    Logger::get()->error_signal.connect([&](std::string message){
        QtLogger::get()->error(QString::fromStdString(message));
    });

    connect(QtLogger::get(), &QtLogger::message_signal, this, [&](QString message){
        m_ui->teLogs->insertHtml(message % QSL("<br>"));
        QTextCursor cursor = m_ui->teLogs->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_ui->teLogs->setTextCursor(cursor);

    });
    connect(QtLogger::get(), &QtLogger::warning_signal, this, [&](QString warning){
        m_ui->teLogs->insertHtml(warning % QSL("<br>"));
        QTextCursor cursor = m_ui->teLogs->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_ui->teLogs->setTextCursor(cursor);
    });
    connect(QtLogger::get(), &QtLogger::error_signal, this, [&](QString error){
        m_ui->teLogs->insertHtml(error % QSL("<br>"));
        QTextCursor cursor = m_ui->teLogs->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_ui->teLogs->setTextCursor(cursor);
    });
    QtLogger::message(QSL("Scaner grabber started the: ") % QDateTime::currentDateTime().toString());


    // read network info
    int readingPort;
    auto networkInfo = K2ConfigFiles::read_grabber_network_config_file(&readingPort, "");
    if(!networkInfo){
        QMessageBox messageBox;
        messageBox.critical(0,QSL("Error"), "Cannot load network config.");
        messageBox.setFixedSize(500,200);
        messageBox.show();
        exit(0);
    }
    // read settings infos
    auto settingsPath  = QString(QDir::currentPath() % QSL("/config/settings/settings_") % QHostInfo::localHostName() % QSL(".config"));
    auto settingsInfos = K2ConfigFiles::read_grabber_settings_config_file(settingsPath.toStdString());

    m_pW.init_from_grabber(readingPort);
    if(settingsInfos.has_value()){
        m_pW.update_ui_settings(settingsInfos.value());
    }
    m_ui->vlLeftMenu->insertWidget(0, &m_pW);

    // init managers
    m_cameraW    = std::make_unique<CameraWorker>();

    // init workers
    m_tcpReader  = std::make_unique<TcpReaderWorker>();
    m_udpSenderW = std::make_unique<UdpSenderWorker>();
    m_dataW      = std::make_unique<ProcessDataWorker>();

    // move to thread
    m_udpSenderW->moveToThread(&m_udpSenderT);
    m_cameraW->moveToThread(&m_cameraT);
    m_dataW->moveToThread(&m_dataT);

    // start processess
    m_udpSenderT.start(QThread::HighPriority);
    m_cameraT.start(QThread::HighPriority);
    m_dataT.start();

    // set connections
    init_connections();

    // # add display widgets
    // ## color
    m_ui->tColor->setLayout(new QHBoxLayout());
    m_ui->tColor->layout()->addWidget(&m_colorW);
    // ## depth
    m_ui->fDepth->setLayout(new QHBoxLayout());
    m_ui->fDepth->layout()->addWidget(&m_depthW);
    // ## infra
    m_ui->fInfra->setLayout(new QHBoxLayout());
    m_ui->fInfra->layout()->addWidget(&m_infraW);
    // ## cloud
    m_ui->f3D->setLayout(new QHBoxLayout());
    m_ui->f3D->layout()->addWidget(&m_depthCameraW);

    // update parameters from ui
    m_pW.send_current_ui_settings();
    m_pW.send_current_ui_display_options();

    // start camera loop
    emit start_camera_loop_signal();

    // start tcp reading
    m_tcpReader->enable_reading(readingPort);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 300);
}


ScanerGrabberWindow::~ScanerGrabberWindow(){

    // quit camera loop
    {
        emit kill_camera_loop_signal();

        QTimer timer;
        timer.setSingleShot(true);

        QEventLoop loop;
        connect(m_cameraW.get(), &CameraWorker::loop_ended_signal, &loop, &QEventLoop::quit );
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit );
        timer.start(10000);
        loop.exec();
    }

    m_cameraT.quit();
    m_udpSenderT.quit();
    m_dataT.quit();

    m_cameraT.wait();
    m_udpSenderT.wait();
    m_dataT.wait();

    // stop timers
    m_eraseTimeGetDataT.stop();
    m_eraseTimeDisplayDataT.stop();
    m_eraseTimeSendataT.stop();
    m_eraseSendRateT.stop();

    m_tcpReader  = nullptr;
    m_udpSenderW = nullptr;
    m_cameraW    = nullptr;

    QtLogger::clean();
}


void ScanerGrabberWindow::set_process_time(std::int64_t time){
    m_ui->laDisplayDataTime->setText(QString::number(time*0.001) + " ms");
    m_eraseTimeDisplayDataT.start(1000);
}

void ScanerGrabberWindow::save_cloud(){


    QString path = QFileDialog::getSaveFileName(nullptr, "Select cloud file", "", "OBJ file (*.obj)");
    if(path.length() == 0){
        return;
    }

    std::ofstream file;
    std::string name(path.toStdString());
    file.open(name);
    if(file.is_open()){

        if(m_lastFrame->mode == K2FrameRequest::compressed_color_cloud){

    //        if(m_kinectManager.mode() == Kinect2FrameRequest::cloud_colored){
    //            for(size_t ii = 0; ii < m_kinectManager.kinect.currentCloudSize; ++ii){
    //                const auto &p = (*m_kinectManager.kinect.cloudData)[ii];
    //                const auto &c = (*m_kinectManager.kinect.colorData)[ii];
    //                file << "v " << p.x() << " " << p.y() << " " << p.z() << " " << static_cast<float>(c.x()) << " " << static_cast<float>(c.y()) << " " << static_cast<float>(c.z()) << "\n";
    //            }
    //        }else if(m_kinectManager.mode() == Kinect2FrameRequest::mesh_colored){
    //            for(size_t ii = 0; ii < m_kinectManager.kinect.currentCloudSize; ++ii){
    //                const auto &p = (*m_kinectManager.kinect.cloudData)[ii];
    //                const auto &c = (*m_kinectManager.kinect.colorData)[ii];
    //                file << "v " << p.x() << " " << p.y() << " " << p.z() << " " << static_cast<float>(c.x()) << " " << static_cast<float>(c.y()) << " " << static_cast<float>(c.z()) << "\n";
    //            }
    //            for(size_t ii = 0; ii < m_kinectManager.kinect.currentMeshSize; ++ii){
    //                const auto &idTri = (*m_kinectManager.kinect.processedtrianglesData)[ii];
    //                file << "f " << idTri.x() << " " << idTri.y() << " "<< idTri.z() << "\n";
    //            }
    //        }

        }else if(m_lastFrame->mode == K2FrameRequest::compressed_color_mesh){

        }else{
            QtLogger::error("Change mode for saving cloud.");
        }
    }
    file.close();
    return;
}

void ScanerGrabberWindow::start_saving_cloud(){

    if(!m_cameraW->is_initialized()){
        return;
    }

    const auto mode = m_cameraW->mode();
    if(mode == K2FrameRequest::compressed_color_cloud || mode == K2FrameRequest::compressed_color_mesh){
        m_ui->pbStartSavingCloud->setEnabled(false);
        m_ui->pbStopSavingCloud->setEnabled(true);        
        m_savedFrames.clear();
        saveData = true;
    }
}

void ScanerGrabberWindow::stop_saving_cloud(){

    saveData = false;
    m_ui->pbStartSavingCloud->setEnabled(true);
    m_ui->pbStopSavingCloud->setEnabled(false);

    if(m_savedFrames.size() == 0){
        QtLogger::error("No frames saved.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(nullptr, "Select cloud movie file", "", "Txt file (*.txt)");
    if(path.length() == 0){
        return;
    }

    std::ofstream file;
    std::string name(path.toStdString());
    file.open(name);
    if(file.is_open()){

        //    saveIntrinsics = frame.intrinsics;
        //    saveCompressedDepthData.emplace_back(frame.compressedDepthData);
        //    std::vector<char> d(frame.jpegColorSize);
        //    std::copy(frame.compressedImage, frame.compressedImage + frame.jpegColorSize, d.begin());
        //    saveCompressedColorData.emplace_back(std::move(d));
        //    saveCompressedColorSizes.emplace_back(frame.jpegColorSize);
        //    saveCompressedDepthSizes.emplace_back(frame.compressedDepthSize);

//        const auto intr = saveIntrinsics;
//        file << intr[0] << " " << intr[1] << " " << intr[2] << " " << intr[3] << " " << intr[4] << " " << intr[5] << " " << intr[6] << "\n";
//        file << saveNbOfFrames << "\n";

//        for(size_t ii = 0; ii < savedFrames.size(); ++ii){

//            file << ii << " " << saveCompressedColorSizes[ii] << " " << saveCompressedDepthSizes[ii] << "\n";
//            for(size_t jj = 0; jj < saveCompressedColorData[ii].size(); ++jj){
//                file << saveCompressedColorData[ii][jj];
//                if(ii < saveCompressedColorData[ii].size()-1){
//                    file << " ";
//                }
//            }
//            file << "\n";
//            for(size_t jj = 0; jj < saveCompressedDepthData[ii].size(); ++jj){
//                file << saveCompressedDepthData[ii][jj];
//                if(ii < saveCompressedDepthData[ii].size()-1){
//                    file << " ";
//                }
//            }
//            file << "\n";
//        }
    }
    file.close();
}


void ScanerGrabberWindow::new_frame(std::int64_t time, std::shared_ptr<K2Frame> frame){

    // update last frame
    m_lastFrame = frame;

    // send frame to udp
    if(m_sendNextFrame){
        emit send_frame_signal(m_lastCommand, m_lastFrame);
        m_sendNextFrame = false;
    }

    // display frame
    if(m_displayData){
        emit process_and_display_signal(m_lastFrame);
    }

    // add frame to saved frames stack
    if(saveData){
        m_savedFrames.push_back(m_lastFrame);
    }

    // update info ui
    m_ui->laGetDataTime->setText(QString::number(0.001*time) + " ms");
    m_eraseTimeGetDataT.start(1000);

    m_ui->laNbBodies->setText(QString::number(m_depthCameraW.bodyTracked));
    m_ui->laNbJointsTracked->setText(QString::number(m_depthCameraW.nbJointsTracked));
    m_ui->laNbJointsInferred->setText(QString::number(m_depthCameraW.nbJointsInferred));

    m_ui->laNbPointsCloud->setText(QString::number(m_depthCameraW.sizeCloud));
    m_ui->laNbPointsMesh->setText(QString::number(m_depthCameraW.sizeMesh));
}

void ScanerGrabberWindow::read_tcp_command(K2TcpPacket command){

    if(command.cmd == K2TcpCommand::Init){
        QtLogger::message("[TCP] Init connection request", true);
        m_pW.update_grabber_writing_info(std::string(std::begin(command.writingIpv6Address), std::end(command.writingIpv6Address)).c_str(), command.writingPort);
        m_udpSenderW->set_grabber_id(command.idGrabber);
    }else if(command.cmd == K2TcpCommand::OpenCamera){
        QtLogger::message("[TCP] Open camera request", true);
        m_pW.open_camera(command.frameMode);
    }else if(command.cmd == K2TcpCommand::CloseCamera){
        QtLogger::message("[TCP] Close camera request", true);
        m_pW.close_camera();
    }else if(command.cmd == K2TcpCommand::UpdateParameters){
        m_pW.update_ui_settings(command.p);
        m_pW.send_current_ui_settings();
    }else if(command.cmd == K2TcpCommand::AskNewFrame){
        ++m_nbAskFrameReceived;
        m_ui->laAskReceived->setText(QString::number(m_nbAskFrameReceived));
        m_lastCommand = command;
        m_sendNextFrame = true;
        if(!m_cameraW->isReading){
            emit read_new_frame_signal();
        }
    }else if(command.cmd == K2TcpCommand::ShutdownComputer){

        QtLogger::message("Shutdown command received.");
        m_ui->twKinectData->hide();
        {
            // Shutdown instruction. (shutdown -s -t xx）
            QString program = "C:/WINDOWS/system32/shutdown.exe";
            QStringList arguments;
            arguments << "-s";// << "t" << "30";
            QProcess *shutdownProcess = new QProcess();
            shutdownProcess->start(program, arguments);
        }
        QtLogger::message("Quit program.");
        QCoreApplication::quit();


    }else if(command.cmd == K2TcpCommand::RestartComputer){

        QtLogger::message("Restart command received.");
        m_ui->twKinectData->hide();
        {
            //  Restart instruction. (shutdown -r -t xx）
            QString program = "C:/WINDOWS/system32/shutdown.exe";
            QStringList arguments;
            arguments << "-r";
            QProcess *shutdownProcess = new QProcess();
            shutdownProcess->start(program, arguments);
        }
        QtLogger::message("Quit program.");
        QCoreApplication::quit();
    }else if(command.cmd == K2TcpCommand::AskToSaveSettingsFile){
        QtLogger::message("Save settings file command received.");
        auto settingsPath  = QString(QDir::currentPath() % QSL("/config/settings/settings_") % QHostInfo::localHostName() % QSL(".config"));
        emit save_settings_config_file_signal(settingsPath);
    }
}

void ScanerGrabberWindow::update_display_state(bool state){
    m_displayData = state;
}

void ScanerGrabberWindow::set_display_parameters(DisplayOptions parameters){
    m_displayParameters = parameters;
    m_depthCameraW.set_model(m_displayParameters.model);
}

void ScanerGrabberWindow::new_tcp_connection_signal(QString address, QString port){
    QtLogger::message(QSL("[TCP] New connection with (") % address % QSL(":") % port % QSL(")"), true);
}

void ScanerGrabberWindow::tcp_connection_ended(QString address, QString port){
    QtLogger::message(QSL("[TCP] Connection ended with (") % address % QSL(":") % port % QSL(")"), true);
}

void ScanerGrabberWindow::update_tcp_connection_state(QString readingAddress, int readingPort, bool state){
    QtLogger::message(QSL("[TCP] ") % (state ? QSL("Start") : QSL("Not")) % QSL(" reading: (") % readingAddress % QSL(":") % QString::number(readingPort) % QSL(")"), true);
    m_pW.update_reading_connection_state(state);
}

void ScanerGrabberWindow::update_udp_connection_state(QString writingAddress, int writingPort, bool state){
    QtLogger::message(QSL("[UDP] ") % (state ? QSL("Start") : QSL("Not")) % QSL(" writing: (") % writingAddress % QSL(":") % QString::number(writingPort) % QSL(")"), true);
    m_pW.update_writing_connection_state(state);
}

void ScanerGrabberWindow::update_udp_packet_failure(size_t count){
    m_totalPacketFailure += to_int(count);
    m_ui->laSentFailurePackets->setText(QString::number(m_totalPacketFailure));
}

void ScanerGrabberWindow::update_udp_frame_sent(int64_t time){
    m_ui->laSendDataTime->setText(QString::number(time*0.001) % QSL(" ms"));
    m_ui->laFramesSent->setText(QString::number(m_nbFramesSent++));
    m_eraseTimeSendataT.start(1000);
}
void ScanerGrabberWindow::update_udp_nb_bytes_sent(qint64 timeStamp, size_t nbBytesSent){

    int id = 0;
    for(const auto &v : m_sendRate){
        if(timeStamp - v.first  > 1000){
            ++id;
        }else{
            break;
        }
    }
    if(id > 0){
        m_sendRate.erase(std::begin(m_sendRate), std::begin(m_sendRate) + id);
    }
    m_sendRate.emplace_back(std::make_pair(timeStamp, nbBytesSent));

    size_t total = 0;
    size_t nbPackets = 0;
    for(const auto &v : m_sendRate){
        total += v.second;
        ++nbPackets;
    }
    double percentage = total / (125000000.);
    m_ui->pbSentData->setValue(static_cast<int>(percentage*100));
    m_ui->laCurrentSent->setText(QString::number(percentage) % QSL(" (") % QString::number(nbPackets) % QSL(" packets/s): "));
    m_eraseSendRateT.start(1000);
}

void ScanerGrabberWindow::update_cloud_data(K2CloudDisplayData *cloudData){
    m_depthCameraW.set_model(m_displayParameters.model);
    m_depthCameraW.sizePtsCloud = m_displayParameters.sizePtsCloud;
    m_depthCameraW.update_cloud(cloudData);
}

void ScanerGrabberWindow::update_mesh_data(K2MeshDisplayData *meshData){
    m_depthCameraW.set_model(m_displayParameters.model);
    m_depthCameraW.update_mesh(meshData);
}

void ScanerGrabberWindow::erase_time_get_data_timeout(){
    m_ui->laGetDataTime->setText("... ms");
}

void ScanerGrabberWindow::erase_time_display_timeout(){
    m_ui->laDisplayDataTime->setText("... ms");
}

void ScanerGrabberWindow::erase_time_send_data_timeout(){
    m_ui->laSendDataTime->setText("... ms");
}

void ScanerGrabberWindow::erase_time_send_rate_timeout(){
    m_ui->pbSentData->setValue(0);
    m_ui->laCurrentSent->setText("0");
}

void ScanerGrabberWindow::save_camera_intrinsics(){
    auto path = QFileDialog::getSaveFileName(nullptr, "Select camera intrinsics file", QString(), "Txt (*.txt)");
    if(path.length() == 0){
        return;
    }
    emit save_camera_intrinsics_signal(path);
}

void ScanerGrabberWindow::save_camera_depth_space_table(){
    auto path = QFileDialog::getSaveFileName(nullptr, "Select camera depth space table file", QString(), "Config (*.config)");
    if(path.length() == 0){
        return;
    }
    emit save_camera_space_depth_table_signal(path);
}

void ScanerGrabberWindow::save_settings_config_file(){
    auto path = QFileDialog::getSaveFileName(nullptr, "Save settings config file", QString(), "Config (*.config)");
    if(path.length() == 0){
        return;
    }
    emit save_settings_config_file_signal(path);
}

void ScanerGrabberWindow::load_settings_config_file(){

    auto path = QFileDialog::getOpenFileName(nullptr, "Load settings config file", QString(), "Config (*.config)");
    if(path.length() == 0){
        return;
    }

    auto settingsInfo = K2ConfigFiles::read_grabber_settings_config_file(path.toStdString());
    if(settingsInfo.has_value()){
        m_pW.update_ui_settings(settingsInfo.value());
    }
}

void ScanerGrabberWindow::set_meta_types(){

    // register types
    qRegisterMetaType<size_t>("std::int64_t");
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<QHostAddress>("QHostAddress");

    qRegisterMetaType<K2TcpPacket>("K2TcpPacket");
    qRegisterMetaType<K2TcpPacket>("cam::K2TcpPacket");
    qRegisterMetaType<K2TcpPacket>("tool::cam::K2TcpPacket");

    qRegisterMetaType<std::shared_ptr<cam::K2Frame>>("std::shared_ptr<cam::K2Frame>");

    qRegisterMetaType<K2Settings>("K2Settings");
    qRegisterMetaType<K2Settings>("cam::K2Settings");
    qRegisterMetaType<K2Settings>("tool::cam::K2Settings");

    qRegisterMetaType<DisplayOptions>("DisplayOptions");
    qRegisterMetaType<DisplayOptions>("ui::DisplayOptions");
    qRegisterMetaType<DisplayOptions>("tool::ui::DisplayOptions");

    qRegisterMetaType<DisplayTimes>("DisplayTimes");
    qRegisterMetaType<DisplayTimes>("ui::DisplayTimes");
    qRegisterMetaType<DisplayTimes>("tool::ui::DisplayTimes");

    qRegisterMetaType<K2FrameRequest>("K2FrameRequest");
    qRegisterMetaType<K2FrameRequest>("K2FrameRequest");
    qRegisterMetaType<K2FrameRequest>("cam::K2FrameRequest");
    qRegisterMetaType<K2FrameRequest>("tool::cam::K2FrameRequest");

    qRegisterMetaType<std::array<K2BodyJoint, 25*6>>("std::array<K2BodyJoint, 25*6>");
    qRegisterMetaType<std::array<K2BodyJoint, 25*6>>("std::array<K2BodyJoint, 25*6>");
    qRegisterMetaType<std::array<K2BodyJoint, 25*6>>("std::array<cam::K2BodyJoint, 25*6>");
    qRegisterMetaType<std::array<K2BodyJoint, 25*6>>("std::array<tool::cam::K2BodyJoint, 25*6>");
}


void ScanerGrabberWindow::init_connections(){

    using GW = GrabberParametersW;
    using CW = CameraWorker;
    using TCP = TcpReaderWorker;
    using UDP = UdpSenderWorker;
    using SSW = ScanerGrabberWindow;
    using GPW = GrabberParametersW;
    using PDW = ProcessDataWorker;
    auto camM = m_cameraW.get();
    auto tcpW = m_tcpReader.get();
    auto udpW = m_udpSenderW.get();
    auto dataW = m_dataW.get();

    //    // from logger
    //    connect(Logger::get(), &Logger::message_signal, &m_pW, &GW::add_network_log_message);
    //    connect(Logger::get(), &Logger::error_signal, &m_pW, &GW::add_network_log_message);

    // from ui
    connect(m_ui->pbStartSavingCloud,                   &QPushButton::clicked, this, &SSW::start_saving_cloud);
    connect(m_ui->pbStopSavingCloud,                    &QPushButton::clicked, this, &SSW::stop_saving_cloud);
    connect(m_ui->actionSave_camera_intrinsics,         &QAction::triggered,   this, &SSW::save_camera_intrinsics);
    connect(m_ui->actionSave_camera_depth_space_table,  &QAction::triggered,   this, &SSW::save_camera_depth_space_table);
    connect(m_ui->actionSave_camera_parameters,         &QAction::triggered,   this, &SSW::save_settings_config_file);
    connect(m_ui->actionLoad_camera_parameters,         &QAction::triggered,   this, &SSW::load_settings_config_file);
    connect(m_ui->actionAbout_scanner_grabber,          &QAction::triggered,   this, [&]{

        Ui_AboutD about;
        QDialog d;
        d.setWindowTitle("About Scanner grabber");
        d.setModal(true);
        about.setupUi(&d);
        connect(about.pbClose, &QPushButton::clicked, &d, &QDialog::close);
        d.exec();
    });

    // from grabber parameters ui
    // # camera
    connect(&m_pW, &GW::open_camera_signal,            camM,  &CW::open_camera);
    connect(&m_pW, &GW::close_camera_signal,           camM,  &CW::close_camera);
    connect(&m_pW, &GW::send_settings_parameters_signal, camM,  &CW::update_camera_parameters);
    connect(&m_pW, &GW::set_data_state_signal,         camM,  &CW::set_loop_state);
    connect(&m_pW, &GW::set_display_state_signal,      this,  &SSW::update_display_state);
    // # -> tcp
    connect(&m_pW, &GW::send_grabber_reading_connection_parameters_signal,  tcpW, &TCP::enable_reading);
    connect(&m_pW, &GW::disable_connection_signal,                        tcpW, &TCP::disable_reading);
    // # -> udp
    connect(&m_pW, &GW::send_grabber_writing_connection_parameters_signal,  udpW, &UDP::enable_writing);
    connect(&m_pW, &GW::disable_connection_signal,                        udpW, &UDP::disable_writing);
    // # -> this
    connect(&m_pW, &GW::send_display_options_signal,                   this, &SSW::set_display_parameters);

    // from this
//    connect(this, &SSW::start_camera_loop_signal,             camM,     &CW::start_loop);
    connect(this, &SSW::kill_camera_loop_signal,              camM,     &CW::kill_loop);
    connect(this, &SSW::save_camera_intrinsics_signal,        camM,     &CW::save_camera_intrisics);
    connect(this, &SSW::save_camera_space_depth_table_signal, camM,     &CW::save_camera_space_depth_table);
    connect(this, &SSW::save_settings_config_file_signal,     camM,     &CW::save_settings_config_file);
    connect(this, &SSW::read_new_frame_signal,                camM,     &CW::manager_read_frame);
    connect(this, &SSW::send_frame_signal,                    udpW,     &UDP::send_frame);
    connect(this, &SSW::process_and_display_signal,           dataW,    &PDW::process_and_display);

    // from
    connect(m_cameraW.get(), &CW::new_frame_signal,    this,   &SSW::new_frame);
    connect(m_cameraW.get(), &CW::camera_opened_signal, &m_pW,  &GPW::set_camera_state_ui);

    // from tcp
    connect(tcpW, &TCP::new_connection_signal,      this, &SSW::new_tcp_connection_signal);
    connect(tcpW, &TCP::connection_ended_signal,    this, &SSW::tcp_connection_ended);
    connect(tcpW, &TCP::connected_state_signal,     this, &SSW::update_tcp_connection_state);
    connect(tcpW, &TCP::tcp_packet_received_signal, this, &SSW::read_tcp_command);

    // from udp
    connect(udpW, &UDP::connected_state_signal,   this, &SSW::update_udp_connection_state);
    connect(udpW, &UDP::packets_failure_signal,   this, &SSW::update_udp_packet_failure);
    connect(udpW, &UDP::frame_sent_signal,        this, &SSW::update_udp_frame_sent);
    connect(udpW, &UDP::nb_bytes_sent_signal,     this, &SSW::update_udp_nb_bytes_sent);

    // from data
    connect(dataW, &PDW::process_time_signal,       this,               &SSW::set_process_time);
    connect(dataW, &PDW::update_cloud_data_signal,  this,               &SSW::update_cloud_data);
    connect(dataW, &PDW::update_mesh_data_signal,   this,               &SSW::update_mesh_data);
    connect(dataW, &PDW::update_joint_data_signal,  &m_depthCameraW,    &DepthCameraGlWidget::update_joints_state);
    connect(dataW, &PDW::update_color_signal,       &m_colorW,          &ImageViewer::set_image);
    connect(dataW, &PDW::update_depth_signal,       &m_depthW,          &ImageViewer::set_image);
    connect(dataW, &PDW::update_infra_signal,       &m_infraW,          &ImageViewer::set_image);

    // from timers
    connect(&m_eraseTimeGetDataT,      &QTimer::timeout,  this, &SSW::erase_time_get_data_timeout);
    connect(&m_eraseTimeDisplayDataT,  &QTimer::timeout,  this, &SSW::erase_time_display_timeout);
    connect(&m_eraseTimeSendataT,      &QTimer::timeout,  this, &SSW::erase_time_send_data_timeout);
    connect(&m_eraseSendRateT,         &QTimer::timeout,  this, &SSW::erase_time_send_rate_timeout);
}

#include "moc_scaner_grabber_window.cpp"

