

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

// std
#include <cstdint>
#include <string>

// local
#include "thirdparty/sigslot/signal.hpp"
#include "settings/dc_color_settings.hpp"
#include "settings/dc_data_settings.hpp"
#include "settings/dc_delay_settings.hpp"
#include "settings/dc_config_settings.hpp"
#include "settings/dc_filters_settings.hpp"
#include "dc_compressed_frame.hpp"

namespace tool::camera {

class DCDevice{
public:

    virtual ~DCDevice(){}

    virtual auto open(std::uint32_t deviceId) -> bool{static_cast<void>(deviceId);return false;}
    virtual auto refresh_devices_list() -> void{}
    virtual auto close() -> void{}
    virtual auto clean() -> void{}

    // getters
    virtual auto nb_devices() const noexcept -> std::uint32_t{return 0;}
    virtual auto device_id() const noexcept -> std::uint32_t{return 0;};
    virtual auto device_name() const noexcept -> std::string{return "";}
    virtual auto is_opened() const noexcept -> bool{return false;}
    virtual auto cameras_started() const noexcept -> bool{return false;}
    virtual auto is_sync_in_connected() const noexcept -> bool{return false;} // TODO: ORBBEC IMPL
    virtual auto is_sync_out_connected() const noexcept -> bool{return false;} // TODO: ORBBEC IMPL
    virtual auto get_nb_capture_per_second() const noexcept -> float{return 0.f;} // TODO: ORBBEC IMPL
    virtual auto get_capture_duration_ms() noexcept -> std::int64_t{return 0; } // TODO: ORBBEC IMPL
    virtual auto get_processing_duration_ms() noexcept -> std::int64_t{return 0;} // TODO: ORBBEC IMPL
    virtual auto get_compressing_duration_ms() noexcept -> std::int64_t{return 0;} // TODO: ORBBEC IMPL
    virtual auto get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::int64_t{static_cast<void>(from);static_cast<void>(to);return 0;} // TODO: ORBBEC IMPL
    virtual auto get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::int64_t{static_cast<void>(from);static_cast<void>(to);return 0;} // TODO: ORBBEC IMPL

    // # config
    virtual auto mode() const noexcept -> DCMode{return {};}  // TODO: ORBBEC IMPL
    virtual auto is_LED_disabled() const noexcept -> bool{return false;}  // TODO: ORBBEC IMPL
    virtual auto synch_mode() const noexcept -> DCSynchronisationMode{return {};}  // TODO: ORBBEC IMPL
    virtual auto subordinate_delay_usec() const noexcept -> int{return 0;}  // TODO: ORBBEC IMPL
    virtual auto color_and_depth_synchronized() const noexcept -> bool{return false;}  // TODO: ORBBEC IMPL
    virtual auto delay_between_color_and_depth_usec() const noexcept -> int{return 0;}  // TODO: ORBBEC IMPL

    // cameras
    virtual auto start_cameras(const DCConfigSettings &configS) -> bool{static_cast<void>(configS);return false;} // TODO: ORBBEC IMPL
    virtual auto stop_cameras() -> void{} // TODO: ORBBEC IMPL

    // settings
    virtual auto set_color_settings(const DCColorSettings &colorS) -> void{static_cast<void>(colorS);}
    virtual auto set_data_settings(const DCDataSettings &dataS) -> void{static_cast<void>(dataS);} // TODO: ORBBEC IMPL
    virtual auto set_filters(const DCFiltersSettings &filters) -> void{static_cast<void>(filters);} // TODO: ORBBEC IMPL
    virtual auto send_data_state(bool state) -> void{static_cast<void>(state);} // TODO: ORBBEC IMPL
    virtual auto set_delay(DCDelaySettings delay) -> void{static_cast<void>(delay);} // TODO: ORBBEC IMPL

    // signals
    sigslot::signal<std::shared_ptr<DCFrame>> new_frame_signal;
    sigslot::signal<std::shared_ptr<DCCompressedFrame>> new_compressed_frame_signal;
    sigslot::signal<DCImuSample> new_imu_sample_signal;
};
}
