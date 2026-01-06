
/*******************************************************************************
** Toolset-ts-network                                                         **
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


// boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace tool::net{
//using socket_type = boost::asio::ip::tcp::socket;

//class connection_handler {
//public:
//    void handle(std::shared_ptr<socket_type> socket, boost::system::error_code& ec);
//};

//class server {
//public:
//    explicit server(boost::asio::io_service& ios, const uint16_t port);
//    ~server();
//    void start();
//    void stop();
//private:
//    void run();
//    std::unique_ptr<std::thread> main_loop_thread_;

//    boost::asio::io_service& ios_;
//    boost::asio::ip::tcp::endpoint endpoint_;
//    boost::asio::ip::tcp::acceptor acceptor_;
//    connection_handler handler_;
//    std::atomic<bool> is_running{false};
//};



class TcpSession{

public:

    TcpSession(boost::asio::io_service& io_service) : socket_(io_service) {
    }

    boost::asio::ip::tcp::socket& socket();
    void start();
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);

private:
    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class TcpServer{

public:
    TcpServer(boost::asio::io_service& io_service, short port);

    void handle_accept(TcpSession* new_session, const boost::system::error_code& error);

private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};
}
