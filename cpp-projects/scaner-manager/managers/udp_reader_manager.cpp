
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

#include "udp_reader_manager.hpp"

// Qt
#include <QThread>
#include <QCoreApplication>

// base
#include "utility/types.hpp"

using namespace tool;
using namespace tool::network;

struct UdpReaderManager::Impl{

    QTextEdit *logsW = nullptr;
    std::vector<std::unique_ptr<QThread>> readersT;
};


// TODO: add enabled interfaces id
UdpReaderManager::UdpReaderManager(const std::vector<Interface> &interfaces, QTextEdit *logsW) : m_p(std::make_unique<Impl>()){

    m_p->logsW = logsW;

    // init interface readers and threads
    for(size_t ii = 0; ii < interfaces.size(); ++ii){

        const auto &interface = interfaces[ii];
        auto readerT = std::make_unique<QThread>();
        auto reader = std::make_unique<InterfaceUdpReceiverWorker>(ii, interface);
        reader->setObjectName("udp_reader_worker_" + QString(interface.ipAddress.c_str()));
        reader->moveToThread(readerT.get());
        readerT->setObjectName("udp_reader_thread_" + QString(interface.ipAddress.c_str()));
        readerT->start(QThread::Priority::HighestPriority);

        readers.emplace_back(std::move(reader));
        m_p->readersT.emplace_back(std::move(readerT));
        readingConnectionState.emplace_back(false);
    }

    // manager connections
    connect(this, &UdpReaderManager::kill_workers_signal, this, [&]{
        for(auto &reader : readers) {
            reader->disable_reading();
        }
    });

    // set connection for each reader
    for(auto &reader : readers) {
        auto readerPtr = reader.get();
//        connect(reader.get(), &InterfaceUdpReceiverWorker::start_reading_signal, reader.get(), &InterfaceUdpReceiverWorker::read_data);
        connect(reader.get(), &InterfaceUdpReceiverWorker::message_signal, this, [=](QString messsage){
            m_p->logsW->append("[" + QString::number(readerPtr->id()) + "] " + messsage);
        });
        connect(reader.get(), &InterfaceUdpReceiverWorker::connected_state_signal, this, [=](bool state){
            readingConnectionState[to_size_t(readerPtr->id())] = state;
            if(state){
                m_p->logsW->append("[" + QString::number(readerPtr->id()) + "] connection established.");
            }else{
                m_p->logsW->append("[" + QString::number(readerPtr->id()) + "] connection aborted.");
            }
        });
        connect(reader.get(), &InterfaceUdpReceiverWorker::timeout_packet_signal, this, [=](){
            m_p->logsW->append("[" + QString::number(readerPtr->id()) + "] timeout");
        });
        connect(reader.get(), &InterfaceUdpReceiverWorker::packet_received_signal,      this,   &UdpReaderManager::packet_received_signal);
        connect(reader.get(), &InterfaceUdpReceiverWorker::frame_dropped_signal,        this,   &UdpReaderManager::frame_dropped_signal);
        connect(reader.get(), &InterfaceUdpReceiverWorker::all_packets_received_signal, this,   &UdpReaderManager::all_packets_received_signal);
    }

    // enable reading for all availale interfaces
    for(auto &reader : readers) {
        reader->enable_reading(8888);
    }
}

UdpReaderManager::~UdpReaderManager(){
    readers.clear();
    m_p = nullptr;
}

void UdpReaderManager::quit_threads(){

    emit kill_workers_signal();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    for(auto &t : m_p->readersT){
        t->quit();
    }
}

void UdpReaderManager::wait_for_threads(){
    for(auto &t : m_p->readersT){
        t->wait();
    }
    m_p->readersT.clear();
}

#include "moc_udp_reader_manager.cpp"
