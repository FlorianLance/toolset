
/*******************************************************************************
** Toolset-base                                                               **
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

#include "tcp_server.hpp"

using namespace tool::net;

boost::asio::ip::tcp::socket &TcpSession::socket(){
    return socket_;
}

void TcpSession::start(){

    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        boost::bind(&TcpSession::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
    ));
}

void TcpSession::handle_read(const boost::system::error_code &error, size_t bytes_transferred){

    if (!error){
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(data_, bytes_transferred),
            boost::bind(&TcpSession::handle_write, this,
            boost::asio::placeholders::error)
        );
    } else{
        delete this;
    }
}

void TcpSession::handle_write(const boost::system::error_code &error){

    if (!error){
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            boost::bind(&TcpSession::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        ));
    }else{
        delete this;
    }
}

TcpServer::TcpServer(boost::asio::io_service &io_service, short port) :
    io_service_(io_service),
    acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)){

    TcpSession* new_session = new TcpSession(io_service_);
    acceptor_.async_accept(
        new_session->socket(),
        boost::bind(&TcpServer::handle_accept, this, new_session,
        boost::asio::placeholders::error
    ));
}

void TcpServer::handle_accept(TcpSession *new_session, const boost::system::error_code &error){

    if (!error){
        new_session->start();
        new_session = new TcpSession(io_service_);
        acceptor_.async_accept(
            new_session->socket(),
            boost::bind(&TcpServer::handle_accept, this, new_session,
            boost::asio::placeholders::error
        ));
    }else{
        delete new_session;
    }
}
