
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

#include "scaner_manager_window.h"
#include "ui_ManagerW.h"

// std
#include <array>
#include <filesystem>

// Qt
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QHostAddress>
#include <QHostInfo>
#include <QDir>

// base
#include "geometry/point4.hpp"
#include "geometry/matrix4.hpp"
#include "geometry/transformation.hpp"
#include "utility/types.hpp"
#include "utility/io_fstream.hpp"
#include "utility/logger.hpp"
#include "depth-camera/impl/k2_config_files.hpp"

// qt_utility
#include "qt_logger.hpp"
#include "qt_str.hpp"
#include "qt_process.hpp"

using namespace tool;
using namespace tool::ui;
using namespace tool::cam;
using namespace tool::net;
using namespace tool::geo;

ScanerManagerWindow::ScanerManagerWindow(Protocol interfaceProtocol)  {

    QThread::currentThread()->setObjectName("main_thread");

    // set ui
    m_ui = new Ui::ManagerW();
    m_ui->setupUi(this);

    // register types
    qRegisterMetaType<std::shared_ptr<cam::K2Frame>>("std::shared_ptr<cam::K2Frame>");
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<std::int64_t>("std::int64_t");
    qRegisterMetaType<std::uint32_t>("uint32_t");
    qRegisterMetaType<std::uint32_t>("std::uint32_t");
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<QTextCursor>("QTextCursor");

    qRegisterMetaType<K2Settings>("K2Settings");
    qRegisterMetaType<K2Settings>("video::K2Settings");
    qRegisterMetaType<K2Settings>("cam::K2Settings");

    qRegisterMetaType<DisplayOptions>("DisplayOptions");
    qRegisterMetaType<DisplayOptions>("ui::DisplayOptions");
    qRegisterMetaType<DisplayOptions>("tool::ui::DisplayOptions");

    qRegisterMetaType<K2FrameRequest>("K2FrameRequest");
    qRegisterMetaType<K2FrameRequest>("video::K2FrameRequest");
    qRegisterMetaType<K2FrameRequest>("cam::K2FrameRequest");

    qRegisterMetaType<K2UdpHeader>("K2UdpHeader");
    qRegisterMetaType<K2UdpHeader>("cam::K2UdpHeader");
    qRegisterMetaType<K2UdpHeader>("tool::cam::K2UdpHeader");

    qRegisterMetaType<K2TcpPacket>("K2TcpPacket");
    qRegisterMetaType<K2TcpPacket>("cam::K2TcpPacket");
    qRegisterMetaType<K2TcpPacket>("tool::cam::K2TcpPacket");

    qRegisterMetaType<std::vector<QByteArray*>>("std::vector<QByteArray*>");
    qRegisterMetaType<std::vector<QByteArray*>>("std::vector<QByteArray*>");

    // init logger
    Logger::no_file_init();
    QtLogger::init("", QSL("scaner_manager_") % QHostInfo::localHostName() % QSL(".html"), true);
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
        m_ui->teManagerLogs->insertHtml(message % QSL("<br>"));
        m_ui->teManagerLogs->verticalScrollBar()->setValue(m_ui->teManagerLogs->verticalScrollBar()->maximum());
    });
    connect(QtLogger::get(), &QtLogger::warning_signal, this, [&](QString warning){
        m_ui->teManagerLogs->insertHtml(warning % QSL("<br>"));
        m_ui->teManagerLogs->verticalScrollBar()->setValue(m_ui->teManagerLogs->verticalScrollBar()->maximum());
    });
    connect(QtLogger::get(), &QtLogger::error_signal, this, [&](QString error){
        m_ui->teManagerLogs->insertHtml(error % QSL("<br>"));
        m_ui->teManagerLogs->verticalScrollBar()->setValue(m_ui->teManagerLogs->verticalScrollBar()->maximum());
    });
    QtLogger::message(QSL("Scaner manager started the: ") % QDateTime::currentDateTime().toString());

    // search for interfaces
    localInterfaces = Interface::list_local_interfaces(interfaceProtocol);

    const QString currentPath = QDir::currentPath();

    // read network info
    const QString currentComputerNetworkConfig = currentPath % QSL("/config/network/network_") % QHostInfo::localHostName() % QSL(".config");
    auto networkInfos = K2ConfigFiles::read_manager_network_config_file(currentComputerNetworkConfig.toStdString());
    if(networkInfos.size() == 0){
        QMessageBox messageBox;
        messageBox.critical(0,QSL("Error"), QString::fromStdString("Cannot load network config."));
        messageBox.setFixedSize(500,200);
        messageBox.show();
        exit(0);
    }

    // read calibrations infos
    const QString currentComputerCalibrationConfig = currentPath % QSL("/config/calibration/calibration_") % QHostInfo::localHostName() % QSL(".config");
    auto calibrationInfos = K2ConfigFiles::read_manager_calibration_file(currentComputerCalibrationConfig.toStdString());
    if(calibrationInfos.size() == 0){
        QMessageBox messageBox;
        messageBox.critical(0,QSL("Error"), QString::fromStdString("Cannot load calibration"));
        messageBox.setFixedSize(500,200);
        messageBox.show();
        exit(0);
    }

    m_multiDepthCameraW = new MultiDepthCameraGlWidget();
    m_multiDepthCameraW->init(networkInfos.size());
    init_managers(networkInfos);
    init_ui(networkInfos);
    init_connections();

    // read settings infos
    QString commonSettingsPath = currentPath % QSL("/config/settings/settings_common.config");
    if(auto commonSettings = K2ConfigFiles::read_grabber_settings_config_file(commonSettingsPath.toStdString()); commonSettings.has_value()){
        m_allCamerasW.update_ui_settings(commonSettings.value());
    }

    QString commonCalibSettingsPath = currentPath % QSL("/config/settings/settings_common_calibration.config");
    if(auto commonCalibSettings = K2ConfigFiles::read_grabber_settings_config_file(commonCalibSettingsPath.toStdString()); commonCalibSettings.has_value()){
        m_allCamerasCalibW.update_ui_settings(commonCalibSettings.value());
    }

    std::vector<K2Settings> individualGrabbersSettings;
    const QString currentComputerBaseSetingsConfig = currentPath % QSL("/config/settings/settings_individual_") % QHostInfo::localHostName() % QSL("_");
    for(size_t ii = 0; ii < networkInfos.size(); ++ii){
        const QString currentComputerSetingsConfig = currentComputerBaseSetingsConfig % QString::number(ii) % QSL(".config");
        auto grabberSettingsInfos = K2ConfigFiles::read_grabber_settings_config_file(currentComputerSetingsConfig.toStdString());
        if(grabberSettingsInfos.has_value()){
            individualGrabbersSettings.push_back(grabberSettingsInfos.value());
        }
    }


    // set individual parameters
    size_t id = 0;
    for(auto &worker : m_grabbersManager){
        if(individualGrabbersSettings.size() == m_grabbersManager.size()){
            QtLogger::message("update_ui_settings");
            worker->update_current_ui_settings(individualGrabbersSettings[id]);
        }
        ++id;
    }

    // set calibrations
    for(size_t ii = 0; ii < calibrationInfos.size(); ++ii){
        if(ii < m_grabbersManager.size()){
            m_grabbersManager[ii]->update_model_matrix(calibrationInfos[ii]);
        }else{
            break;
        }
    }

    emit m_ui->sbSizePackets->valueChanged(m_ui->sbSizePackets->value());
}

ScanerManagerWindow::~ScanerManagerWindow(){

    // stop timers
    m_askFrameTimer.stop();

    // quit every process
    quit_threads();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // wait for every process
    wait_for_threads();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    m_grabbersManager.clear();
    m_udpReaderManager = nullptr;

    delete m_ui;
}

void ScanerManagerWindow::init_managers(const std::vector<K2GrabberTargetInfo> &infos){
    
    m_udpReaderManager = std::make_unique<K2UdpReaderManager>(localInterfaces, m_ui->teNetworkInfoUdpReader);
    m_calibrationManager = std::make_unique<CalibrationManager>(&m_grabbersManager);


    std::vector<QColor> cloudColors = {
        Qt::red,Qt::gray,Qt::green,Qt::yellow,Qt::blue,Qt::black,Qt::cyan,Qt::darkGreen,Qt::darkRed,Qt::magenta,
        Qt::darkYellow, Qt::darkBlue, Qt::darkMagenta, Qt::darkCyan, Qt::lightGray, Qt::darkGray, Qt::white
    };

    for(size_t ii = 0; ii < infos.size(); ++ii){
        m_grabbersManager.emplace_back(std::make_unique<GrabberController>(m_udpReaderManager.get(), &localInterfaces, ii, infos[ii], cloudColors[ii%infos.size()]));
    }
}

void ScanerManagerWindow::init_ui(const std::vector<K2GrabberTargetInfo> &infos){

    setWindowTitle("Scaner-manager");
    resize(QSize(2100, 1500));
    m_ui->fCloudDisplay->setLayout(new QHBoxLayout());
    m_ui->fCloudDisplay->layout()->addWidget(m_multiDepthCameraW);
    m_ui->fCloudDisplay->layout()->setContentsMargins(0,0,0,0);

    for (const auto& m : all_requests_names()) {
        m_ui->cbCameraMode->addItem(from_view(m));
    }

    m_ui->tAllData->setCurrentIndex(1);
    m_ui->tCameras->tabBar()->setStyleSheet("QTabBar::tab { font: bold; min-height: 3em; }");
    m_ui->sbAskFPS->setValue(45);

    QStringList items;
    for(size_t ii = 0; ii < infos.size(); ++ii){
        items << QString::number(ii);
    }
    m_ui->cbParamsListFrom->addItems(items);
    m_ui->cbRegisterModelList->addItems(items);
    items << "All";
    m_ui->cbConnectionList->addItems(items);
    m_ui->cbCamList->addItems(items);
    m_ui->cbCloudList->addItems(items);
    m_ui->cbParamsListTo->addItems(items);
    m_ui->cbCamListFocus->addItems(items);
    m_ui->cbApplyCalibList->addItems(items);

    m_ui->cbCamListFocus->setCurrentText("All");
    m_ui->cbConnectionList->setCurrentText("All");
    m_ui->cbCamList->setCurrentText("All");
    m_ui->cbCloudList->setCurrentText("All");
    m_ui->cbParamsListTo->setCurrentText("All");
    m_ui->cbApplyCalibList->setCurrentText("All");


    // add image viewers to ui
    const int width = 4;
    for(size_t ii = 0; ii < infos.size(); ++ii){
        auto grabberFrame = new QFrame();
        grabberFrame->setFrameShadow(QFrame::Plain);
        grabberFrame->setLineWidth(1);
        grabberFrame->setFrameStyle(QFrame::StyledPanel);
        auto layout = new QHBoxLayout();
        grabberFrame->setLayout(layout);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(&m_grabbersManager[ii]->imageViewer);
        m_ui->glData->addWidget(grabberFrame,static_cast<int>(ii/width), ii%width);
    }

    // add parameters widgets to ui
    std::vector<QWidget*> tabs;
    for(size_t ii = 0; ii < infos.size(); ++ii){
        auto grabberTab= new QWidget();
        auto layout = new QHBoxLayout();
        grabberTab->setLayout(layout);
        layout->setContentsMargins(2,2,2,2);
        layout->addWidget(&m_grabbersManager[ii]->grabberParametersW);
        m_ui->tCameras->insertTab(static_cast<int>(1 + ii), grabberTab, QString::number(ii));
        tabs.emplace_back(m_ui->tCameras->widget(static_cast<int>(1 + ii)));
    }

    // force all camera w
    m_ui->tcFA->setLayout(new QHBoxLayout());
    m_ui->tcFA->layout()->setContentsMargins(2,2,2,2);
    m_ui->tcFA->layout()->addWidget(&m_allCamerasW);
    m_allCamerasW.init_force_all_cameras();


    // force all calib camera w
    m_ui->tcFAC->setLayout(new QHBoxLayout());
    m_ui->tcFAC->layout()->setContentsMargins(2,2,2,2);
    m_ui->tcFAC->layout()->addWidget(&m_allCamerasCalibW);
    m_allCamerasCalibW.init_force_all_cameras_calibration();


    for(size_t ii = 0; ii < infos.size(); ++ii){
        m_ui->lwGrabbersNames->addItem(QString{infos[ii].name.c_str()});
    }
    for(size_t ii = 0; ii < localInterfaces.size(); ++ii){
        m_ui->lwLocalInterfaces->addItem(QString{localInterfaces[ii].ipAddress.c_str()});
    }
}

void ScanerManagerWindow::quit_threads(){

    m_udpReaderManager->quit_threads();

    for(auto &w : m_grabbersManager){
        w->quit_threads();
    }
}

void ScanerManagerWindow::wait_for_threads(){

    m_udpReaderManager->wait_for_threads();

    for(auto &w : m_grabbersManager){
        w->wait_for_threads();
    }
}

void ScanerManagerWindow::init_connections(){

    {// from ui
        connect(m_ui->rbCommon, &QPushButton::clicked, [&]{
            send_all_settings();
        });
        connect(m_ui->rbCommonCalib, &QPushButton::clicked, [&]{
            send_all_settings();
        });
        connect(m_ui->rbIndividual, &QPushButton::clicked, [&]{
            send_all_settings();
        });

        connect(&m_allCamerasW, &GrabberParametersW::send_settings_parameters_signal, this, [&](cam::K2Settings settings){
            if(m_ui->rbCommon->isChecked()){
                auto settings = m_allCamerasW.read_settings_from_ui();
                for(auto &worker : m_grabbersManager){
                    worker->send_ui_settings(settings);
                }
            }
        });
        connect(&m_allCamerasCalibW, &GrabberParametersW::send_settings_parameters_signal, this, [&](cam::K2Settings settings){
            if(m_ui->rbCommonCalib->isChecked()){
                auto settings = m_allCamerasCalibW.read_settings_from_ui();
                for(auto &worker : m_grabbersManager){
                    worker->send_ui_settings(settings);
                }
            }
        });

        connect(m_ui->pbTabNetwork, &QPushButton::clicked, this, [&]{
            for(auto &w : m_grabbersManager){
                w->grabberParametersW.set_parameters_tab_index(0);
            }
        });
        connect(m_ui->pbTabCamera, &QPushButton::clicked, this, [&]{
            for(auto &w : m_grabbersManager){
                w->grabberParametersW.set_parameters_tab_index(1);
            }
        });
        connect(m_ui->pbTabModel, &QPushButton::clicked, this, [&]{
            for(auto &w : m_grabbersManager){
                w->grabberParametersW.set_parameters_tab_index(1);
            }
        });
        connect(m_ui->pbTabFilter, &QPushButton::clicked, this, [&]{
            for(auto &w : m_grabbersManager){
                w->grabberParametersW.set_parameters_tab_index(3);
            }
        });
        connect(m_ui->pbTabDisplay, &QPushButton::clicked, this, [&]{
            for(auto &w : m_grabbersManager){
                w->grabberParametersW.set_parameters_tab_index(4);
            }
        });
        connect(m_ui->pbCopyParamsList, &QPushButton::clicked, this, [&]{
            for(auto &w : m_grabbersManager){
                if(m_ui->cbCloudList->currentText() == "All" || static_cast<size_t>(m_ui->cbCloudList->currentIndex()) == w->idC){
                    w->copy_current_ui_settings();
                }
            }
        });
        connect(m_ui->pbDisplayCloudList,    &QPushButton::clicked, [&]{
            for(auto &w : m_grabbersManager){
                if(m_ui->cbCloudList->currentText() == "All" || static_cast<size_t>(m_ui->cbCloudList->currentIndex()) == w->idC){
                    w->display_cloud();
                }
            }
        });
        connect(m_ui->pbForceColor,    &QPushButton::clicked, [&]{
            for(auto &w : m_grabbersManager){
                if(m_ui->cbCloudList->currentText() == "All" || static_cast<size_t>(m_ui->cbCloudList->currentIndex()) == w->idC){
                    w->force_color_cloud(true);
                }
            }
        });
        connect(m_ui->pbRealColor,    &QPushButton::clicked, [&]{
            for(auto &w : m_grabbersManager){
                if(m_ui->cbCloudList->currentText() == "All" || static_cast<size_t>(m_ui->cbCloudList->currentIndex()) == w->idC){
                    w->force_color_cloud(false);
                }
            }
        });
        connect(m_ui->pbHideCloudList,    &QPushButton::clicked, [&]{
            for(auto &w : m_grabbersManager){
                if(m_ui->cbCloudList->currentText() == "All" || static_cast<size_t>(m_ui->cbCloudList->currentIndex()) == w->idC){
                    w->hide_cloud();
                }
            }
        });
        connect(m_ui->dsbSizeClouds,      QOverload<qreal>::of(&QDoubleSpinBox::valueChanged), [&]{
            for(auto &w : m_grabbersManager){
                if(m_ui->cbCloudList->currentText() == "All" || static_cast<size_t>(m_ui->cbCloudList->currentIndex()) == w->idC){
                    w->set_size_cloud(m_ui->dsbSizeClouds->value());
                }
            }
        });
//        connect(ui->pbCalibSettings,    &QPushButton::clicked, [&]{
//            for(auto &w : m_grabbersManager){
//                w->set_calibration_settings();
//            }
//        });
//        connect(ui->pbDefaultSettings,    &QPushButton::clicked, [&]{
//            for(auto &w : m_grabbersManager){
//                w->set_default_settings();
//            }
//        });
        connect(m_ui->pbAskOneFrame,      &QPushButton::clicked, this, [&]{
            for(auto &w : m_grabbersManager){
                w->ask_new_frame();
            }
        });


        connect(m_ui->sbSizePackets,      QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int value){
            for(auto &w : m_grabbersManager){
                emit w->update_size_udp_packet_signal(value);
            }
        });

    }
    {// from timer
        {// -> this
            connect(&m_askFrameTimer, &QTimer::timeout, this, [&]{
                for(auto &w : m_grabbersManager){
                    w->ask_new_frame();
                    auto currenTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - w->lastTimeReceived).count();

                    if(w->m_wConnection){
                        if(currenTime > 1000){
                            m_ui->tCameras->tabBar()->setTabTextColor(to_int(w->idC+1), Qt::darkYellow);
                        }else{
                            m_ui->tCameras->tabBar()->setTabTextColor(to_int(w->idC+1), Qt::darkGreen);
                        }
                    }else{
                        m_ui->tCameras->tabBar()->setTabTextColor(to_int(w->idC+1), Qt::red);
                    }
                }
            });
        }
    }

    // connections
    for(auto &worker : m_grabbersManager){

        connect(worker.get(), &GrabberController::copy_settings_parameters_signal, this, &ScanerManagerWindow::copy_camera_parameters);

        // From worker
        // # -> this
        connect(worker.get(), &GrabberController::timeout_signal, this, [&]{
            ++m_totalTimeout;
            m_ui->laTimeout->setText("Timeout nb: " + QString::number(m_totalTimeout));
        });
        // # -> display
        connect(worker.get(),   &GrabberController::update_display_parameters_signal, this, [&](size_t id, DisplayOptions p){
            m_multiDepthCameraW->update_grabber_cloud_display_parameters(id, p);
        });
        connect(worker.get(),   &GrabberController::update_cloud_data_signal, this, [&](size_t id, K2CloudDisplayData *cloudData){
            m_multiDepthCameraW->update_cloud(id, cloudData);
        });
        connect(worker.get(),   &GrabberController::update_bodies_data_signal, this, [&](size_t id, std::array<K2BodyInfos,6> *bodiesData){
            m_multiDepthCameraW->update_bodies(id, bodiesData);
        });
        connect(worker.get(),   &GrabberController::update_mesh_data_signal, this, [&](size_t id, K2MeshDisplayData *meshData){
            m_multiDepthCameraW->update_mesh(id, meshData);
        });
        connect(worker.get(),   &GrabberController::update_registers_clouds_data_signal, this, [&](size_t id, std::vector<geo::Pt3f> *calibCloudData, std::vector<geo::Pt3f> *calibColorsData){
            m_multiDepthCameraW->update_raw_cloud(id, calibCloudData, calibColorsData);
        });
        connect(worker.get(),   &GrabberController::writing_connected_state_signal, this, [&](bool state){
            m_ui->tCameras->tabBar()->setTabTextColor(to_int(worker->idC+1), state ? Qt::darkGreen : Qt::red);

            if(state){
                if(m_ui->rbIndividual->isChecked()){
                    QtLogger::message(QString("Send individual ui settings to worker with id: ") + QString::number(worker->idC));
                    worker->send_current_ui_settings();
                }else if(m_ui->rbCommon->isChecked()){
                    QtLogger::message(QString("Send common ui settings to worker with id: ") + QString::number(worker->idC));
                    worker->send_ui_settings(m_allCamerasW.read_settings_from_ui());
                }else{
                    QtLogger::message(QString("Send common-calib ui settings to worker with id: ") + QString::number(worker->idC));
                    worker->send_ui_settings(m_allCamerasCalibW.read_settings_from_ui());
                }
                worker->send_current_ui_display_options();
            }
        });

        connect(worker.get(),   &GrabberController::new_frame_to_save_signal, this, [&](size_t camId, std::shared_ptr<K2Frame> frame){
            m_savedData.emplace_back(std::make_tuple(camId, frame));
            m_ui->laSizeVideoRecording->setText(QString::number(m_savedData.size()));
        });
    }

    // grabber connection
    connect(m_ui->pbConnectList,     &QPushButton::clicked, this, &ScanerManagerWindow::connect_grabbers);
    connect(m_ui->pbDisconnectList,  &QPushButton::clicked, this, &ScanerManagerWindow::disconnect_grabbers);
    connect(m_ui->pbShutdownList,    &QPushButton::clicked, this, &ScanerManagerWindow::shutdown_computers);
    connect(m_ui->pbRestartList,     &QPushButton::clicked, this, &ScanerManagerWindow::restart_computers);
    // # tcp
    connect(m_ui->pbOpenCameraList,  &QPushButton::clicked, this,&ScanerManagerWindow::open_cameras);
    connect(m_ui->pbCloseCameraList, &QPushButton::clicked, this,&ScanerManagerWindow::close_cameras);
    // cameras
    connect(m_ui->cbCamListFocus,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ScanerManagerWindow::focus_camera);

    // view
    connect(m_ui->pbToggleGrid, &QPushButton::clicked, this, [&]{
        m_multiDepthCameraW->displayGrid = !m_multiDepthCameraW->displayGrid;
        m_multiDepthCameraW->update();
    });
    connect(m_ui->pbToggleAxes, &QPushButton::clicked, this, [&]{
        m_multiDepthCameraW->displayAxes = !m_multiDepthCameraW->displayAxes;
        m_multiDepthCameraW->update();
    });
    connect(m_ui->pbToggleJoints, &QPushButton::clicked, this, [&]{
        m_multiDepthCameraW->displayJoints = !m_multiDepthCameraW->displayJoints;
        m_multiDepthCameraW->update();
    });

    // settings
    connect(m_ui->actionAskGrabbersToSaveSettingsFile, &QAction::triggered,  this, [&](){
        for(auto &w : m_grabbersManager){
            QtLogger::message(QSL("Ask grabber ") % QString::number(w->idC) % QSL(" to save settings config file."));
            w->ask_to_save_settings_file();
        }
    });

    // calibration
    connect(m_ui->actionSave_calibration_to_file,         &QAction::triggered,    this, &ScanerManagerWindow::save_calibration);
    connect(m_ui->actionLoad_calibration_file,            &QAction::triggered,    this, &ScanerManagerWindow::load_calibration_config_file);
    connect(m_ui->actionSave_cameras_parameters_to_files, &QAction::triggered,    this, &ScanerManagerWindow::save_grabbers_settings_config_files);    
    connect(m_ui->pbRegisterClouds,                       &QPushButton::clicked,  this, &ScanerManagerWindow::register_clouds);

    connect(m_ui->cbShowCalibration, &QCheckBox::clicked, this, [&]{
        for(auto &worker : m_grabbersManager){
            worker->showCalibration = m_ui->cbShowCalibration->isChecked();
        }
    });
    connect(m_ui->cbOnlyOneCloud, &QCheckBox::clicked, this, [&]{
        for(auto &worker : m_grabbersManager){
            worker->registerOneCloud = m_ui->cbOnlyOneCloud->isChecked();
        }
    });
    connect(m_ui->cbUseProcessed, &QCheckBox::clicked, this, [&]{
        for(auto &worker : m_grabbersManager){
            worker->useProcessed = m_ui->cbUseProcessed->isChecked();
        }
    });

    connect(m_ui->pbProcess, &QPushButton::clicked, this, [&](){
        for(auto &worker : m_grabbersManager){
            // update processing parameters
            worker->computeSphereCenter     = m_ui->cbComputeSphereCenter->isChecked();
            worker->maxDistanceOutlier      = m_ui->dsbMaxDistanceOutlier->value();
            worker->offsetDepth             = static_cast<float>(m_ui->dsbOffsetD->value());

            worker->maxDistanceCorr         = m_ui->dsbMaxCorr->value();
            worker->removeOutliers          = m_ui->cbRemoveOutliers->isChecked();
//            worker->factorCancel            = static_cast<float>(ui->dsbFactorCancel->value());

//                    dsbMaxCorr
            // process
            worker->process_registered_clouds();
        }
    });

    connect(m_ui->pbCalibrate, &QPushButton::clicked, this, &ScanerManagerWindow::calibrate);

    connect(m_ui->pbStartStopVideoRecord, &QPushButton::clicked, this, [&]{

        // laSizeVideoRecording
        if(saveVideo){
            m_ui->pbStartStopVideoRecord->setText("Start cloud video recording");
        }else{
            m_ui->pbStartStopVideoRecord->setText("Stop cloud video recording");
        }

        saveVideo = !saveVideo;
        for(auto &grabber : m_grabbersManager){
            grabber->set_video_saving_state(saveVideo);
        }
    });
    connect(m_ui->pbSaveVideoFile, &QPushButton::clicked, this,  &ScanerManagerWindow::save_video_file);
    connect(m_ui->pbClearVideoFile, &QPushButton::clicked, this, [&]{m_savedData.clear();});

    // fps
    connect(m_ui->sbAskFPS,  QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int value){Q_UNUSED(value)
        emit set_loop_ask_frames_state_signal(m_ui->cbLoopFrames->isChecked());
    });
    connect(m_ui->cbLoopFrames, &QCheckBox::stateChanged, this, [&](int state){Q_UNUSED(state)
        emit set_loop_ask_frames_state_signal(m_ui->cbLoopFrames->isChecked());
    });
    connect(this, &ScanerManagerWindow::set_loop_ask_frames_state_signal, this, &ScanerManagerWindow::set_loop_ask_frames_state);
}


void ScanerManagerWindow::set_loop_ask_frames_state(bool state){

    m_askFrameTimer.stop();
    if(state){
        m_askFrameTimer.start(1000/m_ui->sbAskFPS->value());
    }else{
        m_askFrameTimer.stop();
    }
}


void ScanerManagerWindow::copy_camera_parameters(K2Settings p){
    for(size_t ii = 0; ii < m_grabbersManager.size(); ++ii){
        if((m_ui->cbParamsListTo->currentText() == "All" ||static_cast<size_t>(m_ui->cbParamsListTo->currentIndex()) == ii) &&(m_ui->cbParamsListFrom->currentIndex() != to_int(ii))){
            m_grabbersManager[ii]->update_current_ui_settings(p);
        }
    }
    send_all_settings();
}

void ScanerManagerWindow::focus_camera(int index){
    int idR = index/4,idC = index%4;
    if(m_ui->cbCamListFocus->currentText() == "All"){
        idR=-1;idC=-1;
    }

    for(int ii = 0; ii < m_ui->glData->rowCount(); ++ii){
        m_ui->glData->setRowStretch(ii, ii == idR ? 10 : 0);
    }
    for(int ii = 0; ii < m_ui->glData->columnCount(); ++ii){
        m_ui->glData->setColumnStretch(ii, ii == idC ? 10 : 0);
    }
}


void ScanerManagerWindow::save_grabbers_settings_config_files(){

    auto dirPath = QFileDialog::getExistingDirectory(nullptr, "Select directory where to put grabbers settings config files", QString());
    if(dirPath.length() == 0){
        return;
    }

    size_t id = 0;
    for(const auto &camW : m_grabbersManager){
        QString currentPath = dirPath % QSL("/settings_individual_") % QHostInfo::localHostName() % QSL("_") % QString::number(id++) % QSL(".config");
        K2ConfigFiles::save_grabber_settings_config_file(camW->grabberParametersW.read_settings_from_ui(), currentPath.toStdString());
    }
}

void ScanerManagerWindow::save_video_file(){

    auto path = QFileDialog::getSaveFileName(nullptr, "Select kinect video file", QString(), "Kinect video (*.kvid)");
    if(path.length() == 0){
        return;
    }

    std::ofstream file;
    file.open(path.toStdString(), std::ios_base::binary);
    if(!file.is_open()){
        QtLogger::error("Cannot save video to " + path);
        return;
    }

    // retrieve intrinsics
    std::unordered_map<size_t, std::vector<float>> intrinsics;
    for(auto &data : m_savedData){
        auto idC = std::get<0>(data);
        if(intrinsics.count(idC) == 0){
            intrinsics[idC] = std::get<1>(data)->intrinsics;
        }
    }

    // write total nb of clouds
    write(file, m_savedData.size());

    // write nb of cameras
    write(file, m_grabbersManager.size());

    for(size_t ii = 0; ii < m_grabbersManager.size(); ++ii){

        // write camera transformation
        write(file, m_grabbersManager[ii]->get_model_matrix());

        // write cameras intrinsics
        if(intrinsics.count(ii) != 0){
            auto &camIntrinsics = intrinsics[ii];
            // 0 focalLengthX
            // 1 focalLengthY
            // 2 principalPointX
            // 3 principalPointY
            write_array(file, camIntrinsics.data(), 4);
        }else{ // defaults
            std::vector<float> defaultI = {1.f,1.f,1.f,1.f};
            write_array(file, defaultI.data(), 4);
        }
    }

    // write clouds data
    for(auto &data : m_savedData){

        // get id
        auto id = std::get<0>(data);

        // get frame
        auto &frameD = std::get<1>(data);
        write(file, frameD->timeStampGetFrame); // timestamp
        write(file, id); // id camera
        write(file, frameD->frameId); // frame id

        // write sizes        
        write(file, frameD->compressedDepthSize);
        write(file, frameD->jpegColorSize);

        // write compressed data
        file.write(reinterpret_cast<char*>(frameD->compressedDepthData.data()), static_cast<std::streamsize>(frameD->compressedDepthSize*4));
        file.write(reinterpret_cast<char*>(frameD->compressedImage.data()), static_cast<std::streamsize>(frameD->jpegColorSize));

        // write bodies
        for(auto &body : frameD->bodiesData){

            write(file, &body.id);
            write(file, static_cast<int>(body.engaged));
            write(file, body.tracked);
            write(file, body.restricted);
            write(file, static_cast<int>(body.leanTracking));
            write(file, static_cast<int>(body.leftHandState));
            write(file, static_cast<int>(body.rightHandState));
            write(file, static_cast<int>(body.leftHandHightConfidence));
            write(file, static_cast<int>(body.rightHandHightConfidence));
            write(file, &body.lean);
            write(file, body.joints.size());

            // write joints
            for(auto &joint : body.joints){
                auto jointT = joint.first;
                write(file, &jointT);
                write(file, &joint.second.pos);
                write(file, &joint.second.rotQuaternion);
                write(file, &joint.second.state);
            }
        }
    }
    file.close();
    QtLogger::message("Video saved at " + path);
}


void ScanerManagerWindow::connect_grabbers(){

    m_ui->pbDisconnectList->setEnabled(false);
    for(auto &grabber : m_grabbersManager){
        if(m_ui->cbConnectionList->currentText() == "All" || static_cast<size_t>(m_ui->cbConnectionList->currentIndex()) == grabber->idC){
            grabber->connect_grabber();
            QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
        }
    }

    wait_process(100);
    m_ui->pbDisconnectList->setEnabled(true);
}


void ScanerManagerWindow::send_all_settings(){

    QtLogger::message("Send ui and display settings.");
    if(m_ui->rbIndividual->isChecked()){
        for(auto &worker : m_grabbersManager){
            worker->send_current_ui_settings();
        }
    }else if(m_ui->rbCommon->isChecked()){
        auto settings = m_allCamerasW.read_settings_from_ui();
        for(auto &worker : m_grabbersManager){
            worker->send_ui_settings(settings);
        }
    }else{
        auto settings = m_allCamerasCalibW.read_settings_from_ui();
        for(auto &worker : m_grabbersManager){
            worker->send_ui_settings(settings);
        }
    }

    for(auto &worker : m_grabbersManager){
        worker->send_current_ui_display_options();
    }
}



void ScanerManagerWindow::disconnect_grabbers(){

    m_ui->pbConnectList->setEnabled(false);
    for(auto &grabber : m_grabbersManager){
        if(m_ui->cbConnectionList->currentText() == "All" || static_cast<size_t>(m_ui->cbConnectionList->currentIndex()) == grabber->idC){
            grabber->disconnect_grabber();
            QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
        }
    }
    wait_process(100);
    m_ui->pbConnectList->setEnabled(true);
}

void ScanerManagerWindow::shutdown_computers(){
    for(auto &grabber : m_grabbersManager){
        if(m_ui->cbConnectionList->currentText() == "All" || static_cast<size_t>(m_ui->cbConnectionList->currentIndex()) == grabber->idC){
            grabber->shutdown_computer();
        }
    }
}

void ScanerManagerWindow::restart_computers(){
    for(auto &grabber : m_grabbersManager){
        if(m_ui->cbConnectionList->currentText() == "All" || static_cast<size_t>(m_ui->cbConnectionList->currentIndex()) == grabber->idC){
            grabber->restart_computer();
        }
    }
}

void ScanerManagerWindow::open_cameras(){

    for(size_t ii = 0; ii < m_grabbersManager.size(); ++ii){
        if(m_ui->cbCamList->currentText() == "All" || static_cast<size_t>(m_ui->cbCamList->currentIndex()) == m_grabbersManager[ii]->idC){
            m_grabbersManager[ii]->open_camera_mode(static_cast<K2FrameRequest>(m_ui->cbCameraMode->currentIndex()));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
}

void ScanerManagerWindow::close_cameras(){

    for(size_t ii = 0; ii < m_grabbersManager.size(); ++ii){
        if(m_ui->cbCamList->currentText() == "All" || static_cast<size_t>(m_ui->cbCamList->currentIndex()) == m_grabbersManager[ii]->idC){
            m_grabbersManager[ii]->close_camera_mode();
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
}

void ScanerManagerWindow::save_calibration(){

    auto path = QFileDialog::getSaveFileName(nullptr, "Select calibration file", QString(), "CALIB (*.calib)");
    if(path.length() == 0){
        return;
    }

    std::ofstream file;

    std::string name(path.toStdString());
    file.open(name);
    if(file.is_open()){
        for(size_t ii = 0; ii < m_grabbersManager.size(); ++ii){
            const auto &m = m_grabbersManager[ii]->get_model_matrix();
            file << "# camera " << ii << "\n";
            file << m.at(0,0) << " " << m.at(0,1) << " " << m.at(0,2) << " " << m.at(0,3) << "\n";
            file << m.at(1,0) << " " << m.at(1,1) << " " << m.at(1,2) << " " << m.at(1,3) << "\n";
            file << m.at(2,0) << " " << m.at(2,1) << " " << m.at(2,2) << " " << m.at(2,3) << "\n";
            file << m.at(3,0) << " " << m.at(3,1) << " " << m.at(3,2) << " " << m.at(3,3) << "\n";
        }
    }
    file.close();
}

void ScanerManagerWindow::load_calibration_config_file(){

    auto path = QFileDialog::getOpenFileName(nullptr, "Select calibration config file", QString(), "CALIB (*.config)");
    if(path.length() == 0){
        return;
    }

    auto calibrationsInfo = K2ConfigFiles::read_manager_calibration_file(path.toStdString());
    if(calibrationsInfo.size() != 0){

        for(size_t ii = 0; ii < calibrationsInfo.size(); ++ii){
            if(ii < m_grabbersManager.size()){
                m_grabbersManager[ii]->update_model_matrix(calibrationsInfo[ii]);
            }else{
                break;
            }
        }
    }
}

void ScanerManagerWindow::calibrate(){

    QtLogger::message("Start calibration.");

    // define template and targets clouds
    size_t indexTarget = static_cast<size_t>(m_ui->cbRegisterModelList->currentIndex());
    std::vector<size_t> indexSources;
    if(m_ui->cbApplyCalibList->currentText() == QSL("All")){
        for(size_t ii = 0; ii < m_grabbersManager.size(); ++ii){
            if(ii!= indexTarget){
                indexSources.push_back(ii);
            }
        }

    }else{
        auto currIndex = static_cast<size_t>(m_ui->cbApplyCalibList->currentIndex());
        if(currIndex != indexTarget){
            indexSources.emplace_back(currIndex);
        }else{
            m_ui->teManagerLogs->insertPlainText(QSL("Source identical to target. Can't do the calibration.\n"));
            m_ui->teManagerLogs->verticalScrollBar()->setValue(m_ui->teManagerLogs->verticalScrollBar()->maximum());
            return;
        }
    }

    if(indexSources.size()==0){
        m_ui->teManagerLogs->insertPlainText(QSL("No sources defined, can't do the calibration.\n"));
        m_ui->teManagerLogs->verticalScrollBar()->setValue(m_ui->teManagerLogs->verticalScrollBar()->maximum());
        return;
    }

    for(const auto &indexSource : indexSources){
        m_ui->teManagerLogs->insertPlainText(QSL("Calibrate ") % QString::number(indexSource) % QSL(" with ") % QString::number(indexTarget) % QSL("\n"));
        m_ui->teManagerLogs->verticalScrollBar()->setValue(m_ui->teManagerLogs->verticalScrollBar()->maximum());
        auto mat = m_calibrationManager->calibrate_open3D(m_ui->cbUseProcessed->isChecked(), indexSource, indexTarget);
        if(mat.has_value()){
            m_grabbersManager[indexSource]->update_model_matrix(mat.value());
        }
    }
}


void ScanerManagerWindow::register_clouds(){
    for(auto &worker : m_grabbersManager){
        worker->registerCloudT.setSingleShot(true);
        worker->registerCloudT.start(m_ui->dsbRegisterTime->value()*1000);
        worker->resetCalibCloud = true;
    }
}



#include "moc_scaner_manager_window.cpp"
