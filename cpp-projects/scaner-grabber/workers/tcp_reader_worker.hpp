
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

// base
#include "depth-camera/impl/k2_network.hpp"

namespace tool::net{

class TcpReaderWorker;
using TcpReaderUP = std::unique_ptr<TcpReaderWorker>;

class TcpReaderWorker : public QObject{
    Q_OBJECT

public:

    TcpReaderWorker();
    ~TcpReaderWorker();

public slots:

    bool enable_reading(int readingPort);
    void disable_reading();

private slots:

    void new_connection();
    void disconnected();
    void ready_read();

private:

    void clean_socket();

signals:

    void tcp_packet_received_signal(cam::K2TcpPacket tcpPacket);

    void connected_state_signal(QString readingAddress, int readingPort, bool state);
    void new_connection_signal(QString address, QString port);
    void connection_ended_signal(QString address, QString port);       

private:

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}

