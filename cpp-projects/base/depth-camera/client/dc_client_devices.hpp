

// /*******************************************************************************
// ** Toolset-base                                                               **
// ** MIT License                                                                **
// ** Copyright (c) [2024] [Florian Lance]                                       **
// **                                                                            **
// ** Permission is hereby granted, free of charge, to any person obtaining a    **
// ** copy of this software and associated documentation files (the "Software"), **
// ** to deal in the Software without restriction, including without limitation  **
// ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
// ** and/or sell copies of the Software, and to permit persons to whom the      **
// ** Software is furnished to do so, subject to the following conditions:       **
// **                                                                            **
// ** The above copyright notice and this permission notice shall be included in **
// ** all copies or substantial portions of the Software.                        **
// **                                                                            **
// ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
// ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
// ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
// ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
// ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
// ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
// ** DEALINGS IN THE SOFTWARE.                                                  **
// **                                                                            **
// ********************************************************************************/

// #pragma once

// // local
// #include "thirdparty/sigslot/signal.hpp"
// #include "depth-camera/settings/dc_client_connection_settings.hpp"
// #include "depth-camera/network/dc_network_types.hpp"
// #include "depth-camera/settings/dc_device_settings.hpp"
// #include "depth-camera/settings/dc_color_settings.hpp"
// #include "depth-camera/settings/dc_filters_settings.hpp"
// #include "depth-camera/settings/dc_delay_settings.hpp"
// #include "depth-camera/frame/dc_compressed_frame.hpp"
// #include "depth-camera/frame/dc_frame.hpp"

// namespace tool::cam {

// template<typename ...arg>
// using SSS = sigslot::signal<arg...>;

// struct DCClientDevices{

//     DCClientDevices();
//     ~DCClientDevices();
    
//     auto initialize(const cam::DCClientConnectionSettings &clientS) -> void;
//     auto clean() -> void;

//     auto init_connection(size_t idG) -> void;
//     auto apply_command(size_t idG, net::Command command) -> void;

//     auto update_device_settings(size_t idG, const cam::DCDeviceSettings &deviceS) -> void;
//     auto update_filters_settings(size_t idG, const cam::DCFiltersSettings &filtersS) -> void;
//     auto update_color_settings(size_t idG, const cam::DCColorSettings &colorS) -> void;
//     auto update_delay_settings(size_t idG, const cam::DCDelaySettings &delayS) -> void;

//     auto devices_nb() const noexcept -> size_t;
//     auto device_connected(size_t idG) const noexcept -> bool;
//     auto is_local(size_t idG) const -> bool;

//     auto read_data_from_network(size_t idG) -> size_t;

//     // signals
//     SSS<size_t, std::int64_t> remote_synchro_signal;
//     SSS<size_t, net::Feedback> remote_feedback_signal;
//     SSS<size_t, net::UdpDataStatus> remote_data_status_signal;
//     SSS<size_t, net::UdpNetworkStatus> remote_network_status_signal;

//     SSS<size_t, std::shared_ptr<cam::DCFrame>> local_frame_signal;
//     SSS<size_t, std::shared_ptr<cam::DCCompressedFrame>> remote_frame_signal;

// private:

//     struct Impl;
//     std::unique_ptr<Impl> i;
// };

// }
