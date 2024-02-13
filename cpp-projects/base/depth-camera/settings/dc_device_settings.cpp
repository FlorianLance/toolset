

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

#include "dc_device_settings.hpp"

// std
#include <format>

// utility
#include "utility/logger.hpp"

using namespace tool::cam;

auto DCDeviceSettings::default_init_for_grabber() -> DCDeviceSettings{
    DCDeviceSettings device;
    device.configS  = DCConfigSettings::default_init_for_grabber();
    device.actionsS = DCActionsSettings::default_init_for_grabber();
    return device;
}

auto DCDeviceSettings::default_init_for_manager() -> DCDeviceSettings{
    DCDeviceSettings device;
    device.configS  = DCConfigSettings::default_init_for_manager();
    device.actionsS = DCActionsSettings::default_init_for_manager();
    return device;
}

DCDeviceSettings::DCDeviceSettings(){
    sType   = io::SettingsType::Device;
    version = io::Version::v1_0;
}

auto DCDeviceSettings::init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void {

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCDeviceSettings::init_from_data: Not enought data space for initializing data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::init_from_data(data, offset, sizeData);
    configS.init_from_data(data, offset, sizeData);
    dataS.init_from_data(data, offset, sizeData);
    actionsS.init_from_data(data, offset, sizeData);
}

auto DCDeviceSettings::write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCDeviceSettings::write_to_data: Not enought data space for writing to data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::write_to_data(data, offset, sizeData);
    configS.write_to_data(data, offset, sizeData);
    dataS.write_to_data(data, offset, sizeData);
    actionsS.write_to_data(data, offset, sizeData);
}

auto DCDeviceSettings::total_data_size() const noexcept -> size_t{
    return
        BaseSettings::total_data_size() +
        configS.total_data_size() +
        dataS.total_data_size() +
        actionsS.total_data_size();
}

