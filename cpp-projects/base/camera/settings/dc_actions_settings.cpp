
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

#include "dc_actions_settings.hpp"

// std
#include <format>

// local
#include "utility/io_data.hpp"
#include "utility/logger.hpp"

using namespace tool::camera;

auto DCActionsSettings::default_init_for_grabber() -> DCActionsSettings{
    DCActionsSettings actions;
    actions.openDevice   = false;
    actions.startReading    = false;
    return actions;
}

auto DCActionsSettings::default_init_for_manager() -> DCActionsSettings{
    DCActionsSettings actions;
    actions.openDevice   = false;
    actions.startReading    = false;
    return actions;
}

DCActionsSettings::DCActionsSettings(){
    sType   = io::SettingsType::Device_actions;
    version = io::Version::v1_0;
}

auto DCActionsSettings::init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCActionsSettings::init_from_data: Not enought data space for initializing from data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::init_from_data(data, offset, sizeData);
    read(openDevice, data, offset, sizeData);
    read(startReading, data, offset, sizeData);
}

auto DCActionsSettings::write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCActionsSettings::write_to_data: Not enought data space for writing to data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::write_to_data(data, offset, sizeData);
    write(openDevice, data, offset, sizeData);
    write(startReading, data, offset, sizeData);
}

auto DCActionsSettings::total_data_size() const noexcept -> size_t{
    return
        BaseSettings::total_data_size() +
           sizeof(openDevice) +
        sizeof(startReading);
}
