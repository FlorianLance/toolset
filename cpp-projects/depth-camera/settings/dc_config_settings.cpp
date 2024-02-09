
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

using namespace tool::cam;

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

DCConfigSettings::DCConfigSettings(){
    sType   = io::SettingsType::Device_config;
    version = io::Version::v1_0;
}

auto DCConfigSettings::init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCConfigSettings::init_from_data: Not enought data space for initializing data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::init_from_data(data, offset, sizeData);
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
    read(enableBodyTracking, data, offset, sizeData);
    read(btOrientation, data, offset, sizeData);
    read(btProcessingMode, data, offset, sizeData);
    read(btGPUId, data, offset, sizeData);
    // others
    read(disableLED, data, offset, sizeData);
}

auto DCConfigSettings::write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCConfigSettings::write_to_data: Not enought data space for writing to data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::write_to_data(data, offset, sizeData);
    // device
    write(typeDevice, data, offset, sizeData);
    write(idDevice, data, offset, sizeData);
    write(mode, data, offset, sizeData);
    // synch
    write(synchronizeColorAndDepth, data, offset, sizeData);
    write(delayBetweenColorAndDepthUsec, data, offset, sizeData);
    write(synchMode, data, offset, sizeData);
    write(subordinateDelayUsec, data, offset, sizeData);
    // body tracking
    write(enableBodyTracking, data, offset, sizeData);
    write(btOrientation, data, offset, sizeData);
    write(btProcessingMode, data, offset, sizeData);
    write(btGPUId, data, offset, sizeData);
    // other
    write(disableLED, data, offset, sizeData);
}

auto DCConfigSettings::total_data_size() const noexcept -> size_t{
    return
        BaseSettings::total_data_size() +
        // device
        sizeof(typeDevice) +
        sizeof(idDevice) +
        sizeof(mode) +
        // synch
        sizeof(synchronizeColorAndDepth) +
        sizeof(delayBetweenColorAndDepthUsec) +
        sizeof(synchMode) +
        sizeof(subordinateDelayUsec) +
        // body tracking
        sizeof(enableBodyTracking) +
        sizeof(btOrientation) +
        sizeof(btProcessingMode) +
        sizeof(btGPUId) +
        // other
        sizeof(disableLED);
}
