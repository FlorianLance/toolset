

/*******************************************************************************
** Toolset-ts-network                                                         **
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

#include "remote_server_settings.hpp"

// base
#include "utility/json_utility.hpp"

using namespace tool;
using namespace tool::net;
using json = nlohmann::json;

auto RemoteServerSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // remote server
    read_and_update_value(json, unreadCount, "id_reading_interface"sv, idReadingInterface);
    read_and_update_value(json, unreadCount, "reading_port"sv, readingPort);
    read_and_update_value(json, unreadCount, "sending_address"sv, sendingAddress);
    read_and_update_value(json, unreadCount, "sending_port"sv, sendingPort);
    protocol = (read_and_return_value<std::string>(json, unreadCount, "protocol"sv) == "ipv6"sv) ? Protocol::ipv6 : Protocol::ipv4;

    if(unreadCount != 0){
        tool::Log::warning(std::format("[RemoteServerSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto RemoteServerSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, Settings::convert_to_json());
    // remote server
    add_value(json, "id_reading_interface"sv,     idReadingInterface);
    add_value(json, "reading_port"sv,             readingPort);
    add_value(json, "sending_address"sv,          isLocalhost ? "localhost"sv : sendingAddress);
    add_value(json, "sending_port"sv,             sendingPort);
    add_value(json, "protocol"sv,                 (protocol == Protocol::ipv6) ? "ipv6"sv : "ipv4"sv);

    return json;
}
