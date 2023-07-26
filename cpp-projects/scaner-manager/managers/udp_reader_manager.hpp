
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
#include <QTextEdit>

// local
#include "workers/interface_udp_receiver_worker.hpp"

namespace tool::network {

class UdpReaderManager;
using UdpReaderManagerUP = std::unique_ptr<UdpReaderManager>;

class UdpReaderManager : public QObject{
    Q_OBJECT
public:

    UdpReaderManager(const std::vector<Interface> &interfaces, QTextEdit *logsW);
    ~UdpReaderManager();

    // threads
    void quit_threads();
    void wait_for_threads();

signals:

    void kill_workers_signal();
    void packet_received_signal(size_t idInterface, int idGrabber, size_t idFrame);
    void frame_dropped_signal(size_t idInterface, int idGrabber, size_t idFrame, size_t packetsDropped);
    void all_packets_received_signal(size_t idInterface, int idGrabber, std::vector<QByteArray*> buffers);

public:

    std::vector<InterfaceUdpReceiverWorkerUP> readers;
    std::vector<bool> readingConnectionState;

private:
    struct Impl;
    std::unique_ptr<Impl> m_p;
};

}
