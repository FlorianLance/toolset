


/*******************************************************************************
** Toolset-dc-grabber                                                  **
** MIT License                                                                **
** Copyright (c) [20224] [Florian Lance]                                       **
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

#include "udp_connection_settings.hpp"

// local
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace tool;
using namespace tool::net;
using namespace tool::data;
using json = nlohmann::json;

auto UdpConnectionSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // settings
    read_and_update_value(json, unreadCount, "address"sv,   address);
    read_and_update_value(json, unreadCount, "port"sv,      port);
    read_and_update_value(json, unreadCount, "max_size_udp_packet"sv,   maxPacketSize);

    if(unreadCount != 0){
        tool::Log::warning(std::format("[UdpConnectionSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto UdpConnectionSettings::convert_to_json() const -> nlohmann::json{
    json json;
    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // settings
    add_value(json, "address"sv,    address);
    add_value(json, "port"sv,       port);
    add_value(json, "max_size_udp_packet"sv,    maxPacketSize);

    return json;
}
