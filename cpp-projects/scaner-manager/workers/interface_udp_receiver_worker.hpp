
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
#include "network/network_utility.hpp"

namespace tool::network{

class InterfaceUdpReceiverWorker;
using InterfaceUdpReceiverWorkerUP = std::unique_ptr<InterfaceUdpReceiverWorker>;


class InterfaceUdpReceiverWorker : public QObject{
    Q_OBJECT
public:

    InterfaceUdpReceiverWorker(size_t id, Interface interface);
    ~InterfaceUdpReceiverWorker();

    size_t id() const;
    Interface interface() const;

public slots:

    void enable_reading(int readingPort);
    void disable_reading();

private slots:

    void read_data();

private:

    void clean_socket();

signals:

    void start_reading_signal();

    void timeout_packet_signal();
    void connected_state_signal(bool);
    void message_signal(QString);

    void packet_received_signal(size_t idInterface, int idGrabber, size_t idFrame);
    void frame_dropped_signal(size_t idInterface, int idGrabber, size_t idFrame, size_t packetsDropped);
    void all_packets_received_signal(size_t idInterface, int idGrabber, std::vector<QByteArray*> buffers);


private :

    struct Impl;
    std::unique_ptr<Impl> m_p;
};
}

