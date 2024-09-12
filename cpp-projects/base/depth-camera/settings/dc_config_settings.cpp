
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
    io::Settings::init_from_json(read_object(json, unreadCount, "base"sv));
    // actions
    openDevice                    = read_value<bool>(json, unreadCount, "open_device"sv);
    startReading                  = read_value<bool>(json, unreadCount, "start_reading"sv);
    // device
    typeDevice                    = static_cast<DCType>(read_value<int>(json, unreadCount, "type_device"sv));
    idDevice                      = read_value<std::uint32_t>(json, unreadCount, "id_device"sv);
    useSerialNumber               = read_value<bool>(json, unreadCount, "use_serial_number"sv);
    serialNumber                  = read_value<std::string>(json, unreadCount, "serial_number_device"sv);
    ipv4Address                   = {
        read_value<std::uint8_t>(json, unreadCount, "ipv4_address_1"sv),
        read_value<std::uint8_t>(json, unreadCount, "ipv4_address_2"sv),
        read_value<std::uint8_t>(json, unreadCount, "ipv4_address_3"sv),
        read_value<std::uint8_t>(json, unreadCount, "ipv4_address_4"sv)
    };
    mode                          = static_cast<DCMode>(read_value<int>(json, unreadCount, "mode"sv));

    // synch
    synchronizeColorAndDepth      = read_value<bool>(json, unreadCount, "synchronize_color_and_depth"sv);
    delayBetweenColorAndDepthUsec = read_value<int>(json, unreadCount, "delay_between_color_and_depth_usec"sv);
    synchMode                     = static_cast<DCSynchronisationMode>(read_value<int>(json, unreadCount, "synch_mode"sv));
    subordinateDelayUsec          = read_value<int>(json, unreadCount, "subordinate_delay_usec"sv);
    // body tracking
    btEnabled                     = read_value<bool>(json, unreadCount, "bt_enabled"sv);
    btOrientation                 = static_cast<DCBTSensorOrientation>(read_value<int>(json, unreadCount, "bt_orientation"sv));
    btProcessingMode              = static_cast<DCBTProcessingMode>(read_value<int>(json, unreadCount, "bt_processing_mode"sv));
    btGPUId                       = read_value<std::int8_t>(json, unreadCount, "bt_GPU_id"sv);
    // misc
    disableLED                    = read_value<bool>(json, unreadCount, "disable_LED"sv);
    // color - depth calibration
    data::read_array<float>(json, unreadCount, "color_alignment_tr"sv, colorAlignmentTr.array);
    data::read_array<float>(json, unreadCount, "color_alignment_rot"sv, colorAlignmentRot.array);

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[{}] values have not been initialized from json data.\n", unreadCount));
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
    add_array<float>(json, "color_alignment_rot"sv,             colorAlignmentRot.array);

    return json;
}

auto DCConfigSettings::default_init_for_grabber() -> DCConfigSettings{
    DCConfigSettings config;

    // TODO
    return config;
}

auto DCConfigSettings::default_init_for_manager() -> DCConfigSettings{
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
        read(colorAlignmentRot, data, offset, sizeData);
    }
    
    version = io::SettingsVersion::LastVersion;
}
