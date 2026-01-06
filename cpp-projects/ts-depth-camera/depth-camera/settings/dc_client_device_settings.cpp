
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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

#include "dc_client_device_settings.hpp"

// local
#include "utility/json_utility.hpp"

using namespace tool::cam;
using json = nlohmann::json;

DCClientDeviceSettings::DCClientDeviceSettings(){
    sType   = SettingsType::Dc_client_device;
    version = SettingsVersion::LastVersion;
}

auto DCClientDeviceSettings::set_id(size_t idC) -> void{

    static constexpr std::array<geo::Pt4f, 12> colors = {
        geo::Pt4f{1.0f,0.0f,0.0f, 1.0f},
        {0.0f,1.0f,0.0f, 1.0f},
        {0.0f,0.0f,1.0f, 1.0f},
        {1.0f,1.0f,0.0f, 1.0f},
        {0.0f,1.0f,1.0f, 1.0f},
        {1.0f,0.0f,1.0f, 1.0f},
        {0.5f,0.5f,0.0f, 1.0f},
        {0.0f,0.5f,0.5f, 1.0f},
        {0.5f,0.0f,0.5f, 1.0f},
        {1.0f,0.5f,0.0f, 1.0f},
        {0.0f,0.5f,1.0f, 1.0f},
        {1.0f,0.0f,0.5f, 1.0f},
    };

    id   = idC;
    if(connectionS.connectionType == DCClientType::Remote){
        name = std::format("R{}", id);
    }else if(connectionS.connectionType == DCClientType::Local){
        name = std::format("L{}", id);
    }
    displayS.unicolor = colors[id];
}

auto DCClientDeviceSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // local
    connectionS.init_from_json(read_and_return_object(json, unreadCount, "connection"sv));
    deviceS.init_from_json(read_and_return_object(json, unreadCount, "device"sv));
    filtersS.init_from_json(read_and_return_object(json, unreadCount, "filters"sv));
    calibrationFiltersS.init_from_json(read_and_return_object(json, unreadCount, "calibration_filters"sv));
    colorS.init_from_json(read_and_return_object(json, unreadCount, "color"sv));
    modelS.init_from_json(read_and_return_object(json, unreadCount, "model"sv));
    miscS.init_from_json(read_and_return_object(json, unreadCount, "misc"sv));
    // displayS.init_from_json(read_object(json, unreadCount, "display"sv));
}

auto DCClientDeviceSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, Settings::convert_to_json());
    // local
    add_value(json, "connection"sv, connectionS.convert_to_json());
    add_value(json, "device"sv, deviceS.convert_to_json());
    add_value(json, "filters"sv, filtersS.convert_to_json());
    add_value(json, "calibration_filters"sv, calibrationFiltersS.convert_to_json());
    add_value(json, "color"sv, colorS.convert_to_json());
    add_value(json, "model"sv, modelS.convert_to_json());
    add_value(json, "misc"sv, miscS.convert_to_json());
    // add_value(json, "display"sv, displayS.convert_to_json());

    return json;
}
