
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

// local
#include "utility/io_data.hpp"

using namespace tool::camera;

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

auto DCDataSettings::init_from_data(std::int8_t *data) -> void{

    size_t offset = 0;
    read(captureAudio, data, offset);
    read(captureIMU, data, offset);
    read(captureBodies, data, offset);

    read(generateRGBLocalFrame, data, offset);
    read(generateDepthLocalFrame, data, offset);
    read(generateInfraLocalFrame, data, offset);
    read(generateCloudLocal, data, offset);

    read(sendColor, data, offset);
    read(sendDepth, data, offset);
    read(sendInfra, data, offset);
    read(sendCloud, data, offset);
    read(sendIMU, data, offset);
    read(sendAudio, data, offset);
    read(sendBodies, data, offset);

    read(btTemporalSmoothing, data, offset);
}

auto DCDataSettings::convert_to_data(std::int8_t *data) const -> void{

    size_t offset = 0;
    write(captureAudio, data, offset);
    write(captureIMU, data, offset);
    write(captureBodies, data, offset);

    write(generateRGBLocalFrame, data, offset);
    write(generateDepthLocalFrame, data, offset);
    write(generateInfraLocalFrame, data, offset);
    write(generateCloudLocal, data, offset);

    write(sendColor, data, offset);
    write(sendDepth, data, offset);
    write(sendInfra, data, offset);
    write(sendCloud, data, offset);
    write(sendIMU, data, offset);
    write(sendAudio, data, offset);
    write(sendBodies, data, offset);

    write(btTemporalSmoothing, data, offset);
}

