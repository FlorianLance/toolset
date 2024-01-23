
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

using namespace tool::cam;

auto DCDataSettings::default_init_for_grabber() -> DCDataSettings{
    DCDataSettings data;
    data.generateRGBLocalFrame   = true;
    data.generateDepthLocalFrame = true;
    data.generateInfraLocalFrame = true;
    data.generateCloudLocal      = true;
    return data;
}

auto DCDataSettings::default_init_for_manager() -> DCDataSettings{
    DCDataSettings data;
    data.generateRGBLocalFrame   = false;
    data.generateDepthLocalFrame = false;
    data.generateInfraLocalFrame = false;
    data.generateCloudLocal      = false;
    return data;
}

DCDataSettings::DCDataSettings(){
    sType   = io::SettingsType::Device_data;
    version = io::Version::v1_0;
}

auto DCDataSettings::init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCDataSettings::init_from_data: Not enought data space for initializing data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::init_from_data(data, offset, sizeData);
    read(captureAudio, data, offset, sizeData);
    read(captureIMU, data, offset, sizeData);
    read(captureBodies, data, offset, sizeData);
    read(generateRGBLocalFrame, data, offset, sizeData);
    read(generateDepthLocalFrame, data, offset, sizeData);
    read(generateInfraLocalFrame, data, offset, sizeData);
    read(generateCloudLocal, data, offset, sizeData);
    read(sendColor, data, offset, sizeData);
    read(sendDepth, data, offset, sizeData);
    read(sendInfra, data, offset, sizeData);
    read(sendCloud, data, offset, sizeData);
    read(sendIMU, data, offset, sizeData);
    read(sendAudio, data, offset, sizeData);
    read(sendBodies, data, offset, sizeData);
    read(btTemporalSmoothing, data, offset, sizeData);
}

auto DCDataSettings::write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCDataSettings::write_to_data: Not enought data space for writing to data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::write_to_data(data, offset, sizeData);
    write(captureAudio, data, offset, sizeData);
    write(captureIMU, data, offset, sizeData);
    write(captureBodies, data, offset, sizeData);
    write(generateRGBLocalFrame, data, offset, sizeData);
    write(generateDepthLocalFrame, data, offset, sizeData);
    write(generateInfraLocalFrame, data, offset, sizeData);
    write(generateCloudLocal, data, offset, sizeData);
    write(sendColor, data, offset, sizeData);
    write(sendDepth, data, offset, sizeData);
    write(sendInfra, data, offset, sizeData);
    write(sendCloud, data, offset, sizeData);
    write(sendIMU, data, offset, sizeData);
    write(sendAudio, data, offset, sizeData);
    write(sendBodies, data, offset, sizeData);
    write(btTemporalSmoothing, data, offset, sizeData);
}

auto DCDataSettings::total_data_size() const noexcept -> size_t{
    return
        BaseSettings::total_data_size() +
        sizeof(captureAudio) +
        sizeof(captureIMU) +
        sizeof(captureBodies) +
        sizeof(generateRGBLocalFrame) +
        sizeof(generateDepthLocalFrame) +
        sizeof(generateInfraLocalFrame) +
        sizeof(generateCloudLocal) +
        sizeof(sendColor) +
        sizeof(sendDepth) +
        sizeof(sendInfra) +
        sizeof(sendCloud) +
        sizeof(sendIMU) +
        sizeof(sendAudio) +
        sizeof(sendBodies)+
        sizeof(btTemporalSmoothing);
}

