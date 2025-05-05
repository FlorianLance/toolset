
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

#include "dc_config_settings.hpp"

// std
#include <format>

// local
#include "utility/io_data.hpp"
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCConfigSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // actions
    read_and_update_value(json, unreadCount, "open_device"sv,   openDevice);
    read_and_update_value(json, unreadCount, "start_reading"sv, startReading);
    // device
    typeDevice          = static_cast<DCType>(read_and_return_value(json, unreadCount, "type_device"sv, static_cast<int>(typeDevice)));
    read_and_update_value(json, unreadCount, "id_device"sv, idDevice);
    read_and_update_value(json, unreadCount, "use_serial_number"sv, useSerialNumber);
    read_and_update_value(json, unreadCount, "serial_number_device"sv, serialNumber);
    ipv4Address = {
        read_and_return_value(json, unreadCount, "ipv4_address_1"sv, ipv4Address.x()),
        read_and_return_value(json, unreadCount, "ipv4_address_2"sv, ipv4Address.y()),
        read_and_return_value(json, unreadCount, "ipv4_address_3"sv, ipv4Address.z()),
        read_and_return_value(json, unreadCount, "ipv4_address_4"sv, ipv4Address.w())
    };
    mode                = static_cast<DCMode>(read_and_return_value(json, unreadCount, "mode"sv, static_cast<int>(mode)));

    // stream
    read_and_update_value(json, unreadCount, "enable_ir_stream"sv, enableIRStream);

    // synch
    read_and_update_value(json, unreadCount, "synchronize_color_and_depth"sv,           synchronizeColorAndDepth);
    read_and_update_value(json, unreadCount, "delay_between_color_and_depth_usec"sv,    delayBetweenColorAndDepthUsec);
    synchMode           = static_cast<DCSynchronisationMode>(read_and_return_value(json, unreadCount, "synch_mode"sv, static_cast<int>(synchMode)));
    read_and_update_value(json, unreadCount, "subordinate_delay_usec"sv,                subordinateDelayUsec);
    // body tracking
    read_and_update_value(json, unreadCount, "bt_enabled"sv, btEnabled);
    btOrientation       = static_cast<DCBTSensorOrientation>(read_and_return_value(json, unreadCount,   "bt_orientation"sv, static_cast<int>(btOrientation)));
    btProcessingMode    = static_cast<DCBTProcessingMode>(read_and_return_value(json, unreadCount,      "bt_processing_mode"sv, static_cast<int>(btProcessingMode)));
    read_and_update_value(json, unreadCount, "bt_GPU_id"sv, btGPUId);
    // misc
    read_and_update_value(json, unreadCount, "disable_LED"sv, disableLED);
    // color - depth calibration
    data::read_and_update_array<float>(json, unreadCount, "color_alignment_tr"sv, colorAlignmentTr.array);
    data::read_and_update_array<float>(json, unreadCount, "color_alignment_rot"sv, colorAlignmentRotEuler.array);

    if(unreadCount != 0){
        tool::Log::warning(std::format("[{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCConfigSettings::convert_to_json() const -> nlohmann::json {

    json json;
    // base
    add_value(json, "base"sv,                                   io::Settings::convert_to_json());
    // actions
    add_value(json, "open_device"sv,                            openDevice);
    add_value(json, "start_reading"sv,                          startReading);
    // device
    add_value(json, "type_device"sv,                            static_cast<int>(typeDevice));
    add_value(json, "use_serial_number"sv,                      useSerialNumber);
    add_value(json, "id_device"sv,                              idDevice);
    add_value(json, "serial_number_device"sv,                   serialNumber);
    add_value(json, "ipv4_address_1"sv,                         ipv4Address.x());
    add_value(json, "ipv4_address_2"sv,                         ipv4Address.y());
    add_value(json, "ipv4_address_3"sv,                         ipv4Address.z());
    add_value(json, "ipv4_address_4"sv,                         ipv4Address.w());
    add_value(json, "mode"sv,                                   static_cast<int>(mode));
    // stream
    add_value(json, "enable_ir_stream"sv,                       enableIRStream);
    // synch
    add_value(json, "synchronize_color_and_depth"sv,            synchronizeColorAndDepth);
    add_value(json, "delay_between_color_and_depth_usec"sv,     delayBetweenColorAndDepthUsec);
    add_value(json, "synch_mode"sv,                             static_cast<int>(synchMode));
    add_value(json, "subordinate_delay_usec"sv,                 subordinateDelayUsec);
    // body tracking
    add_value(json, "bt_enabled"sv,                             btEnabled);
    add_value(json, "bt_orientation"sv,                         static_cast<int>(btOrientation));
    add_value(json, "bt_processing_mode"sv,                     static_cast<int>(btProcessingMode));
    add_value(json, "bt_GPU_id"sv,                              btGPUId);
    // misc
    add_value(json, "disable_LED"sv,                            disableLED);
    // color - depth calibration
    add_array<float>(json, "color_alignment_tr"sv,              colorAlignmentTr.array);
    add_array<float>(json, "color_alignment_rot"sv,             colorAlignmentRotEuler.array);

    return json;
}

auto DCConfigSettings::default_init_for_server() -> DCConfigSettings{
    DCConfigSettings config;

    // TODO
    return config;
}

auto DCConfigSettings::default_init_for_client() -> DCConfigSettings{
    DCConfigSettings config;

    // TODO
    return config;
}


auto DCConfigSettings::init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void{

    Settings::init_from_data(data, offset, sizeData);
    // actions
    read(openDevice, data, offset, sizeData);
    read(startReading, data, offset, sizeData);
    // device
    read(typeDevice, data, offset, sizeData);
    read(idDevice, data, offset, sizeData);
    read(mode, data, offset, sizeData);
    // stream
    read(enableIRStream, data, offset, sizeData);
    // synch
    read(synchronizeColorAndDepth, data, offset, sizeData);
    read(delayBetweenColorAndDepthUsec, data, offset, sizeData);
    read(synchMode, data, offset, sizeData);
    read(subordinateDelayUsec, data, offset, sizeData);
    // body tracking
    read(btEnabled, data, offset, sizeData);
    read(btOrientation, data, offset, sizeData);
    read(btProcessingMode, data, offset, sizeData);
    read(btGPUId, data, offset, sizeData);
    // others
    read(disableLED, data, offset, sizeData);
    
    if(version > io::SettingsVersion::v1_0){
        // color - depth calibration
        read(colorAlignmentTr, data, offset, sizeData);
        read(colorAlignmentRotEuler, data, offset, sizeData);
    }
    
    version = io::SettingsVersion::LastVersion;
}
