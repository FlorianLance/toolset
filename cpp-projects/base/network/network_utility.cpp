


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

#include "network_utility.hpp"

// boost
#include <boost/asio.hpp>
#include <boost/asio/ip/host_name.hpp>

// local
#include "utility/logger.hpp"

using namespace boost::asio;
using namespace tool::network;

std::vector<Interface> Interface::list_local_interfaces(Protocol protocol){

    using namespace boost::asio;
    using namespace boost::asio::ip;

    io_service ioService;
    udp::resolver resolver(ioService);
    udp::resolver::query query(host_name(),"");

    std::vector<Interface> interfaces;
    try{
        udp::resolver::iterator it = resolver.resolve(query);

        while(it!=udp::resolver::iterator()){
            address addr =(it++)->endpoint().address();
            if((addr.is_v6() ? Protocol::ipv6 : (addr.is_v4() ? Protocol::ipv4 : Protocol::unknow)) == protocol){
                interfaces.emplace_back(Interface{protocol,addr.to_string()});
            }
        }
    }catch (const boost::system::system_error &error){
        Logger::error(std::format("list_local_interfaces: Cannot list interfaces, error message: {}\n", error.what()));
    }

    ioService.stop();
    return interfaces;
}

std::string Host::get_name(){
    return boost::asio::ip::host_name();
}
