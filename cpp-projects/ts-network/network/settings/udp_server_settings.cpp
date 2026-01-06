

/*******************************************************************************
** Toolset-dc-grabber                                                  **
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

#include "udp_server_settings.hpp"

// base
#include "utility/logger.hpp"
#include "utility/json_utility.hpp"

using namespace tool;
using namespace tool::net;
using json = nlohmann::json;

auto UdpServerSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // settings
    protocol = (read_and_return_value<std::string>(json, unreadCount, "protocol"sv) == "ipv6"sv) ? Protocol::ipv6 : Protocol::ipv4;
    read_and_update_value(json, unreadCount, "any_reading_interface"sv,   anyReadingInterface);

    if(json.contains("reading_id_interface"sv)){
        read_and_update_value(json, unreadCount, "id_reading_interface"sv,    readingInterfaceId);
    }else{
        read_and_update_value(json, unreadCount, "id_interface"sv,            readingInterfaceId);
    }

    read_and_update_value(json, unreadCount, "use_specific_reading_ip_address"sv, useSpecificReadingIpAddress);

    read_and_update_value(json, unreadCount, "reading_ipv4_0"sv, specificReadingIpv4Address.x());
    read_and_update_value(json, unreadCount, "reading_ipv4_1"sv, specificReadingIpv4Address.y());
    read_and_update_value(json, unreadCount, "reading_ipv4_2"sv, specificReadingIpv4Address.z());
    read_and_update_value(json, unreadCount, "reading_ipv4_3"sv, specificReadingIpv4Address.w());

    read_and_update_value(json, unreadCount, "reading_ipv6_0"sv, specificReadingIpv6Address[0]);
    read_and_update_value(json, unreadCount, "reading_ipv6_1"sv, specificReadingIpv6Address[1]);
    read_and_update_value(json, unreadCount, "reading_ipv6_2"sv, specificReadingIpv6Address[2]);
    read_and_update_value(json, unreadCount, "reading_ipv6_3"sv, specificReadingIpv6Address[3]);
    read_and_update_value(json, unreadCount, "reading_ipv6_4"sv, specificReadingIpv6Address[4]);
    read_and_update_value(json, unreadCount, "reading_ipv6_5"sv, specificReadingIpv6Address[5]);
    read_and_update_value(json, unreadCount, "reading_ipv6_6"sv, specificReadingIpv6Address[6]);
    read_and_update_value(json, unreadCount, "reading_ipv6_7"sv, specificReadingIpv6Address[7]);



    read_and_update_value(json, unreadCount, "reading_port"sv,            readingPort);
    read_and_update_value(json, unreadCount, "max_udp_packet_size"sv,     maxUdpPacketSize);


    if(unreadCount != 0){
        Log::warning(std::format("[UdpServerSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto UdpServerSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, Settings::convert_to_json());
    // settings
    add_value(json, "protocol"sv,                   (protocol == Protocol::ipv6) ? "ipv6"sv : "ipv4"sv);
    add_value(json, "any_reading_interface"sv,      anyReadingInterface);
    add_value(json, "id_reading_interface"sv,       readingInterfaceId);
    add_value(json, "use_specific_reading_ip_address"sv,  useSpecificReadingIpAddress);
    add_value(json, "reading_ipv4_0"sv,  specificReadingIpv4Address.x());
    add_value(json, "reading_ipv4_1"sv,  specificReadingIpv4Address.y());
    add_value(json, "reading_ipv4_2"sv,  specificReadingIpv4Address.z());
    add_value(json, "reading_ipv4_3"sv,  specificReadingIpv4Address.w());
    add_value(json, "reading_ipv6_0"sv,  specificReadingIpv6Address[0]);
    add_value(json, "reading_ipv6_1"sv,  specificReadingIpv6Address[1]);
    add_value(json, "reading_ipv6_2"sv,  specificReadingIpv6Address[2]);
    add_value(json, "reading_ipv6_3"sv,  specificReadingIpv6Address[3]);
    add_value(json, "reading_ipv6_4"sv,  specificReadingIpv6Address[4]);
    add_value(json, "reading_ipv6_5"sv,  specificReadingIpv6Address[5]);
    add_value(json, "reading_ipv6_6"sv,  specificReadingIpv6Address[6]);
    add_value(json, "reading_ipv6_7"sv,  specificReadingIpv6Address[7]);
    add_value(json, "reading_port"sv,               readingPort);    
    add_value(json, "max_udp_packet_size"sv,        maxUdpPacketSize);

    return json;
}
