

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



using namespace tool::camera;

auto DCDeviceSettings::default_init_for_grabber(DCType type) -> DCDeviceSettings{
    DCDeviceSettings device;
    device.configS  = DCConfigSettings::default_init_for_grabber(type);
    device.dataS    = DCDataSettings::default_init_for_grabber();
    device.actionsS = DCActionsSettings::default_init_for_grabber();
    return device;
}

auto DCDeviceSettings::default_init_for_manager(DCType type) -> DCDeviceSettings{
    DCDeviceSettings device;
    device.configS  = DCConfigSettings::default_init_for_manager(type);
    device.dataS    = DCDataSettings::default_init_for_manager();
    device.actionsS = DCActionsSettings::default_init_for_manager();
    return device;
}

auto DCDeviceSettings::init_from_data(std::int8_t *data) -> void {
    configS.init_from_data(data);
    data += configS.total_data_size();
    dataS.init_from_data(data);
    data += dataS.total_data_size();
    actionsS.init_from_data(data);
}

auto DCDeviceSettings::convert_to_data(std::int8_t *data) const -> void{
    configS.convert_to_data(data);
    data += configS.total_data_size();
    dataS.convert_to_data(data);
    data += dataS.total_data_size();
    actionsS.convert_to_data(data);
}
