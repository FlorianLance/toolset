
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

#pragma once

// local
#include "dc_config_settings.hpp"
#include "dc_data_settings.hpp"
#include "dc_actions_settings.hpp"

namespace tool::camera {

    struct DCDeviceSettings : io::BinaryFileSettings{

        DCConfigSettings configS;
        DCDataSettings dataS;
        DCActionsSettings actionsS;

        // local
        std::string filePath;

        static auto default_init_for_grabber() -> DCDeviceSettings;
        static auto default_init_for_manager() -> DCDeviceSettings;

        DCDeviceSettings();
        DCDeviceSettings(std::int8_t const * const data, size_t &offset, size_t sizeData){
            DCDeviceSettings::init_from_data(data, offset, sizeData);
        }

        // i/o
        auto init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void override;
        auto write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void override;
        auto total_data_size() const noexcept -> size_t override;

        static auto save_array_to_file(const std::vector<DCDeviceSettings> &devicesA, const std::string &filePath) -> bool;
        static auto init_array_from_file(std::vector<DCDeviceSettings> &devicesA, const std::string &filePath) -> bool;
    };

}
