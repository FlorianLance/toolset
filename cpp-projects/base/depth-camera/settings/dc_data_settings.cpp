
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

#include "dc_data_settings.hpp"

// std
#include <format>

// local
#include "utility/io_data.hpp"
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCDataSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // server
    server.capture.init_from_json(read_and_return_object(json,     unreadCount, json.contains("s_capture"sv)      ? "s_capture"sv      : "client_capture"sv));
    std::string_view sendingS;
    if(json.contains("s_sending"sv)){
        sendingS = "s_sending"sv;
    }else if(json.contains("s_generation"sv)){
        sendingS = "s_generation"sv;
    }else{
        sendingS = "client_generation"sv;
    }
    server.sending.init_from_json(read_and_return_object(json,     unreadCount, sendingS));
    server.generation.init_from_json(read_and_return_object(json,  unreadCount, json.contains("s_generation"sv)   ? "s_generation"sv   : "client_generation"sv));
    // client
    clientGeneration.init_from_json(read_and_return_object(json,   unreadCount, json.contains("c_generation"sv)   ? "c_generation"sv   : "server_generation"sv));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCFiltersSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCDataSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv,           io::Settings::convert_to_json());
    // sever
    add_value(json, "s_capture"sv,      server.capture.convert_to_json());
    add_value(json, "s_sending"sv,      server.sending.convert_to_json());
    add_value(json, "s_generation"sv,   server.generation.convert_to_json());
    // client
    add_value(json, "c_generation"sv,   clientGeneration.convert_to_json());

    return json;
}

auto DCDataSettings::apply_remote_profile() -> void{

    // server
    server.apply_remote_profile();

    // client
    // # generation
    clientGeneration.depthSizedColorImage    = true;
    clientGeneration.cloud                   = true;
    clientGeneration.originalSizeColorImage  = false;
    clientGeneration.depthImage              = false;
    clientGeneration.infraImage              = false;
    clientGeneration.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
}

auto DCDataSettings::apply_local_profile() -> void{

    // server
    server.apply_local_profile();

    // client
    // # generation
    clientGeneration.cloud                   = false;
    clientGeneration.depthSizedColorImage    = false;    
    clientGeneration.originalSizeColorImage  = false;
    clientGeneration.depthImage              = false;
    clientGeneration.infraImage              = false;
    clientGeneration.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
}

auto DCDataSettings::init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void{

    Settings::init_from_data(data, offset, sizeData);
    read(server, data, offset, sizeData);
    read(clientGeneration, data, offset, sizeData);
    version = io::SettingsVersion::LastVersion;
}



