

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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


#include "udp_server.hpp"

// local


// using namespace tool::net;

// struct NetworkMessage{
//     virtual ~NetworkMessage(){}
// };

// struct DCMessage : public NetworkMessage{
//     // std::variant<std::shared_ptr<cam::DCCompressedFrame>, Feedback>
// };


// auto UdpMessageSender::initialize(const UdpConnectionSettings &udpConnectionS) -> void{

//     // Logger::message(std::format("[UdpMessageSender::initialize] Init sender with sending adress [{}] port [{}] and protocol [{}].\n", networkS->clientConnectionSettings.address, networkS->clientConnectionSettings.port, static_cast<int>(networkS->protocol)));
//     // if(sender->init_socket(udpConnectionS.address, std::to_string(udpConnectionS.port), networkS->protocol)){

//         // sendMessagesT = std::make_unique<std::thread>(&DCUdpServer::send_messages_loop, this);
//         // messagesToSend.push_back(Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received});

//     // }else{
//         // Logger::error(fmt("[DCClientConnection::init_sender] Cannot init sender with address [{]] and port [{}]\n", networkS->clientConnectionSettings.address,std::to_string(networkS->clientConnectionSettings.port)));
//     // }
// }


// struct UdpServer::Impl{

// };

// UdpServer::UdpServer() : i(std::make_unique<Impl>()){

// }

// UdpServer::~UdpServer(){

// }

// auto UdpServer::initialize() -> void{
//     m_reader = generate_reader();
//     init_connections();
// }

// auto UdpServer::start_reading(const UdpServerSettings &udpServerS) -> bool{

//     // stop reading
//     if(m_reader->is_reading_thread_started()){
//         m_reader->stop_reading_thread();
//     }


//     // init reader
//     if(!m_reader->init_socket(udpServerS.udpReadingInterface.ipAddress, udpServerS.udpReadingPort, udpServerS.protocol)){
//         return false;
//     }

//     // start reading
//     m_reader->start_reading_thread();


//     return true;
// }

// auto UdpServer::add_client(const UdpConnectionSettings &udpConnectionS) -> void{
//     UdpMessageSender clientSender;
//     clientSender.sender = generate_sender();
//     clientSender.initialize(udpConnectionS);
//     m_clientsSenders.push_back(std::move(clientSender));
// }

// auto UdpServer::clean() -> void{

//     // // quit sending thread
//     // i->sendMessages = false;
//     // if(i->sendMessagesT != nullptr){
//     //     if(i->sendMessagesT->joinable()){
//     //         i->sendMessagesT->join();
//     //     }
//     //     i->sendMessagesT = nullptr;
//     // }

//     // // clean clients connections
//     // for(auto &clientConnection : i->clientsConnection){
//     //     clientConnection.sender->clean_socket();
//     // }

//     // clean messages to send
//     // ...

//     // stop reading
//     if(m_reader->is_reading_thread_started()){
//         m_reader->stop_reading_thread();
//     }
//     m_reader->clean_socket();

// }

