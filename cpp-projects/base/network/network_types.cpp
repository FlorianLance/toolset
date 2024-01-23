


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

#include "network_types.hpp"

// boost
#include <boost/asio.hpp>
#include <boost/asio/ip/host_name.hpp>

// local
#include "utility/logger.hpp"

using namespace boost::asio;
using namespace tool::net;

auto Host::get_name() -> std::string{
    return boost::asio::ip::host_name();
}


auto Interface::list_local_interfaces(Protocol protocol) -> std::vector<Interface>{

    using namespace boost::asio;
    using namespace boost::asio::ip;

    io_service ioService;
    udp::resolver resolver(ioService);
    udp::resolver::query query(protocol ==  Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4(), host_name(), "");

    std::vector<Interface> interfaces;
    try{
        udp::resolver::iterator it = resolver.resolve(query);

        while(it!=udp::resolver::iterator()){
            address addr = (it++)->endpoint().address();
            interfaces.emplace_back(Interface{protocol,addr.to_string()});
        }
    }catch (const boost::system::system_error &error){
        Logger::error(std::format("list_local_interfaces: Cannot list interfaces, error message: {}\n", error.what()));
    }

    ioService.stop();
    return interfaces;
}


UdpNetworkSendingSettings::UdpNetworkSendingSettings(std::string ipAdressStr, uint16_t port, uint16_t maxSizeUdpPacket) : port(port), maxSizeUdpPacket(maxSizeUdpPacket) {
    std::fill(ipAdress.begin(), ipAdress.end(), ' ');
    if(ipAdressStr.size() <= 45){
        std::copy(std::begin(ipAdressStr), std::end(ipAdressStr), ipAdress.begin());
    }
}
