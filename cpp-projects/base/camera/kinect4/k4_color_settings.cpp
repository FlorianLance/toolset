

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

#include "k4_color_settings.hpp"

// local
#include "utility/io_data.hpp"

using namespace tool::camera;

auto K4ColorSettings::init_from_data(std::int8_t *data) -> void {
    size_t offset = 0;
    read(autoExposureTime, data, offset);
    read<std::int8_t>(exposureTimeAbsolute, data, offset, 0, 5);
    read<std::uint8_t>(brightness, data, offset);
    read<std::uint8_t>(contrast, data, offset, 0, 10);
    read<std::uint8_t>(saturation, data, offset, 0, 63);
    read<std::uint8_t>(sharpness, data, offset, 0, 4);
    read(autoWhiteBalance, data, offset);
    read<std::uint16_t>(whiteBalance, data, offset, 2500, 12500);
    read(backlightCompensation, data, offset);
    read<std::uint8_t>(gain, data, offset);
    std::int8_t plf;
    read(plf, data, offset);
    powerlineFrequency = static_cast<K4PowerlineFrequency>(plf);
}

auto K4ColorSettings::convert_to_data(std::int8_t *data) const -> void{
    size_t offset = 0;
    write(autoExposureTime, data, offset);
    write(exposureTimeAbsolute, data, offset);
    write(brightness, data, offset);
    write(contrast, data, offset);
    write(saturation, data, offset);
    write(sharpness, data, offset);
    write(autoWhiteBalance, data, offset);
    write(whiteBalance, data, offset);
    write(backlightCompensation, data, offset);
    write(gain, data, offset);
    write(static_cast<std::int8_t>(powerlineFrequency), data, offset);
}
