
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

// base
#include "network/network_types.hpp"
#include "depth-camera/impl/k2_types.hpp"

namespace tool::net{

class TcpSenderWorker : public QObject{
    Q_OBJECT
public:

    TcpSenderWorker(size_t id, std::vector<Interface> *localInterfaces);
    ~TcpSenderWorker();

    int reading_interface() const;

public slots:

    void enable_writing(QString targetName, int writingPort, int readingInterface, int readingPort);
    void disable_writing();

    void open_camera_command(cam::K2FrameRequest mode);
    void close_camera_command();
    void update_settings_command(cam::K2Settings p);
    void ask_new_frame_command();
    void ask_to_save_settings_file_command();
    void set_size_udp_packets(int size);


    void shutdown_computer();
    void restart_computer();



signals:

    void connection_state_signal(bool);
    void message_signal(QString message);

private :

    void init_command(int targetWritingPort, std::string targetWritingIpAddress);
    void clean_socket();
    void write_data();

private:

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}

