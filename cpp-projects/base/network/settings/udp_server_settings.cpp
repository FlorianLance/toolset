

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

// local
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace tool;
using namespace tool::net;
using namespace tool::data;
using json = nlohmann::json;

auto UdpServerSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_object(json, unreadCount, "base"sv));
    // settings
    anyReadingInterface  = read_value<bool>(json, unreadCount, "any_reading_interface");
    readingInterfaceId   = read_value<size_t>(json, unreadCount, "id_interface");
    readingPort          = read_value<int>(json, unreadCount, "reading_port");
    protocol                = (read_value<std::string>(json, unreadCount, "protocol") == "ipv6") ? Protocol::ipv6 : Protocol::ipv4;

    if(unreadCount != 0){
        Logger::warning(std::format("[UdpServerSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto UdpServerSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // settings
    add_value(json, "any_reading_interface"sv,  anyReadingInterface);
    add_value(json, "id_interface"sv,           readingInterfaceId);
    add_value(json, "reading_port"sv,           readingPort);
    add_value(json, "protocol"sv,               (protocol == Protocol::ipv6) ? "ipv6" : "ipv4");

    return json;
}
