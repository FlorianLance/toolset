
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

// local
#include "utility/io_data.hpp"

using namespace tool::camera;

auto DCConfigSettings::default_init_for_grabber(DCType typeDevice) -> DCConfigSettings{
    DCConfigSettings config;
    config.typeDevice = typeDevice;
    // TODO
    return config;
}

auto DCConfigSettings::default_init_for_manager(DCType typeDevice) -> DCConfigSettings{
    DCConfigSettings config;
    config.typeDevice = typeDevice;
    // TODO
    return config;
}

auto DCConfigSettings::init_from_data(std::int8_t *data) -> void{
    auto sizeData = total_data_size();
    size_t offset = 0;
    read(idDevice, data, offset);
    read(mode, data, offset);
    read(synchronizeColorAndDepth, data, offset);
    read(delayBetweenColorAndDepthUsec, data, offset);
    read(synchMode, data, offset);
    read(subordinateDelayUsec, data, offset);
    read(disableLED, data, offset);
    read(btOrientation, data, offset);
    read(btProcessingMode, data, offset);
    read(btGPUId, data, offset);
    read(typeDevice, data, offset, sizeData); // TODO: check
}

auto DCConfigSettings::convert_to_data(std::int8_t *data) const -> void{
    size_t offset = 0;
    write(idDevice, data, offset);
    write(mode, data, offset);
    write(synchronizeColorAndDepth, data, offset);
    write(delayBetweenColorAndDepthUsec, data, offset);
    write(synchMode, data, offset);
    write(subordinateDelayUsec, data, offset);
    write(disableLED, data, offset);
    write(btOrientation, data, offset);
    write(btProcessingMode, data, offset);
    write(btGPUId, data, offset);    
    write(typeDevice, data, offset); // TODO: check
}
