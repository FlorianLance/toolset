
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

#include "tcp_reader_worker.hpp"

// Qt
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>
#include <QHostAddress>

// qt-utility
#include "qt_process.hpp"
#include "qt_logger.hpp"

using namespace tool;
using namespace tool::net;
using namespace tool::cam;


struct ConnectionBuffer{
    QByteArray buffer;
    qint32 size = 0;
};


struct TcpReaderWorker::Impl{

    QHash<QTcpSocket*, std::shared_ptr<ConnectionBuffer>> messages;
    std::unique_ptr<QTcpServer> server = nullptr;
    int readingPort;
};

TcpReaderWorker::TcpReaderWorker() : m_p(std::make_unique<Impl>()){
}

TcpReaderWorker::~TcpReaderWorker(){
    disable_reading();
}

void TcpReaderWorker::clean_socket(){

    wait_process(20);

    if(m_p->server){
        m_p->server->close();
    }
    m_p->server = nullptr;
}

bool TcpReaderWorker::enable_reading(int readingPort){

    disable_reading();

    m_p->readingPort = readingPort;
    m_p->server = std::make_unique<QTcpServer>();
    connect(m_p->server.get(), &QTcpServer::newConnection, this, &TcpReaderWorker::new_connection);

    if(!m_p->server->listen(QHostAddress::Any, static_cast<quint16>(m_p->readingPort))){
        clean_socket();
        emit connected_state_signal("Any", m_p->readingPort, false);
        return false;
    }

    emit connected_state_signal("Any", m_p->readingPort, true);
    QtLogger::message("[TCP] Reading enabled");
    return true;
}


void TcpReaderWorker::disable_reading(){

    emit connected_state_signal("Any", m_p->readingPort, false);    
    clean_socket();
    QtLogger::message("[TCP] Reading disabled");
}

void TcpReaderWorker::new_connection(){

    while (m_p->server->hasPendingConnections()){

        QTcpSocket *socket = m_p->server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead,    this, &TcpReaderWorker::ready_read);
        connect(socket, &QTcpSocket::disconnected, this, &TcpReaderWorker::disconnected);
        m_p->messages.insert(socket, std::make_shared<ConnectionBuffer>());
        emit new_connection_signal(socket->localAddress().toString(), QString::number(socket->localPort()));
    }
}

void TcpReaderWorker::disconnected(){

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    m_p->messages.remove(socket);
    emit connection_ended_signal(socket->localAddress().toString(), QString::number(socket->localPort()));
}

qint32 array_to_int(QByteArray source){
    std::reverse(source.begin(), source.end());
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

void TcpReaderWorker::ready_read(){

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());

    if(!m_p->messages.contains(socket)){
        QtLogger::error("Cannot read packet, socket not registered in hash.");
        return;
    }

    ConnectionBuffer *message = m_p->messages.value(socket).get();
    QByteArray *buffer = &message->buffer;
    qint32 *s = &message->size;
    qint32 size = *s;

    while (socket->bytesAvailable() > 0){

        buffer->append(socket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)){ //While can process data, process it

            if (size == 0 && buffer->size() >= 4){ //if size of data has received completely, then store it
                size = array_to_int(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size){ // If data has received completely, then emit our SIGNAL with the data
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;

                // process data
                K2TcpPacket packet;
                std::move(data.begin(), data.begin()+ sizeof(packet), reinterpret_cast<char*>(&packet));
                emit tcp_packet_received_signal(packet);
            }
        }
    }
}


#include "moc_tcp_reader_worker.cpp"


