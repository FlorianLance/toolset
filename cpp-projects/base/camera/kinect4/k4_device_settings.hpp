
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
#include "k4_config_settings.hpp"
#include "k4_data_settings.hpp"
#include "k4_actions_settings.hpp"

namespace tool::camera {

    struct K4DeviceSettings : files::BinaryFileSettings{

        K4ConfigSettings configS;
        K4DataSettings dataS;
        K4ActionsSettings actionsS;

        K4DeviceSettings() = default;
        K4DeviceSettings(std::int8_t *data){K4DeviceSettings::init_from_data(data);}
        ~K4DeviceSettings(){}

        static auto default_init_for_grabber() -> K4DeviceSettings;
        static auto default_init_for_manager() -> K4DeviceSettings;

        // i/o
        auto init_from_data(std::int8_t *data) -> void override;
        auto convert_to_data(std::int8_t *data) const -> void override;
        auto total_data_size() const noexcept -> size_t override{
            return
                configS.total_data_size() +
                dataS.total_data_size() +
                actionsS.total_data_size();
        }
        auto type() const noexcept  -> std::int32_t override {return static_cast<std::int32_t>(SettingsType::Device);};
        auto file_description() const noexcept -> std::string_view override {return settings_name(static_cast<SettingsType>(type()));}

        static auto save_array_to_file(const std::vector<K4DeviceSettings> &devicesA, const std::string &filePath) -> bool;
        static auto init_array_from_file(std::vector<K4DeviceSettings> &devicesA, const std::string &filePath) -> bool;
    };

}
