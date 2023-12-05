

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
#include "dc_server_connection.hpp"

namespace tool::network {

struct DCServerNetwork{

    ~DCServerNetwork();
    auto initialize(const std::vector<network::ReadSendNetworkInfos> &infos) -> bool;
    auto clean() -> void;

    auto init_connection(size_t idG) -> void;
    auto send_command(size_t idG, Command command) -> void;
    auto send_device_settings(size_t idG, const camera::DCDeviceSettings &deviceS) -> void;
    auto send_color_settings(size_t idG, const camera::DCColorSettings &colorS) -> void;
    auto send_filters(size_t idG, const camera::DCFiltersSettings &filters) -> void;
    auto send_delay(size_t idG, camera::DCDelaySettings delay) -> void;

    auto connections_nb() const noexcept -> size_t {return connections.size();}
    auto get_connection(size_t idG) const -> DCServerConnection*{return connections[idG].get();}

    auto do_not_use_global_signals() ->void;

private:
    std::vector<std::unique_ptr<DCServerConnection>> connections;
};

}
