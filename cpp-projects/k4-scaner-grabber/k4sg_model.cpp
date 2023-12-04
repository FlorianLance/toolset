
/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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

#include "k4sg_model.hpp"

// base
#include "utility/logger.hpp"

using namespace tool;

auto K4SGModel::initialize() -> bool{

    connection.init_connections();

    // load settings files
    if(!settings.initialize()){
        return false;
    };

    // start udp reading
    if(!connection.start_reading(&settings.network)){
        return false;
    }

    // device
    Logger::message("Init device reader\n");
    device.initialize(camera::DCType::Kinect4);
    device.update_delay(settings.delay);

    return true;
}

auto K4SGModel::clean() -> void{
    device.clean();
    connection.clean();
}

auto K4SGModel::trigger_settings() -> void{
    // triggers data to init ui
    settings.triggers_filters_settings();
    settings.triggers_display_settings();
    settings.triggers_device_settings();
    settings.triggers_model();
}

