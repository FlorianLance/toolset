
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
#include "io/binary_settings.hpp"

namespace tool::cam {

enum class ClientDataAction : std::int8_t{
    None = 0,               // do not capture/generate
    Process,                // capture/generate
    ProcessSend,            // same as Process + send to server
    ProcessDisplay,         // same as Process + generate displayable data
    ProcessSendDisplay      // same as ProcessSend + generate displayable data
};

enum class ServerDataAction : std::int8_t{
    None = 0,               // received data will be ignored
    Process,                // data will be decoded/copied
    ProcessDisplay          // same as Process + generate displayable data
};

enum class CloudColorMode : std::int8_t{
    FromDepthData = 0,          // use a gradient level to generate colors corresponding to depth
    FromDepthSizedColorImage    // use colors from deph-sized color image
};

enum class CloudGenerationMode : std::int8_t{
    FromDepth = 0,
    FromDecodedCloud
};

[[nodiscard]] static constexpr auto do_process(ClientDataAction cdA) noexcept -> bool{
    return (cdA != ClientDataAction::None);
}

[[nodiscard]] static constexpr auto do_send(ClientDataAction cdA) noexcept -> bool{
    return (cdA == ClientDataAction::ProcessSend) || (cdA == ClientDataAction::ProcessSendDisplay);
}

[[nodiscard]] static constexpr auto do_display(ClientDataAction cdA) noexcept -> bool{
    return (cdA == ClientDataAction::ProcessDisplay) || (cdA == ClientDataAction::ProcessSendDisplay);
}

[[nodiscard]] [[maybe_unused]] static constexpr auto do_process(ServerDataAction sdA) noexcept -> bool{
    return (sdA != ServerDataAction::None);
}

[[nodiscard]] [[maybe_unused]] static constexpr auto do_display(ServerDataAction cdA) noexcept -> bool{
    return (cdA == ServerDataAction::ProcessDisplay);
}

struct DCClientDataActions{

    ClientDataAction color             = ClientDataAction::Process;
    ClientDataAction depth             = ClientDataAction::ProcessSend;
    ClientDataAction infra             = ClientDataAction::None;
    ClientDataAction bodiesIdMap       = ClientDataAction::None;
    ClientDataAction audio             = ClientDataAction::None;
    ClientDataAction IMU               = ClientDataAction::None;
    ClientDataAction bodyTracking      = ClientDataAction::None;
    ClientDataAction depthSizedColor   = ClientDataAction::None;
    ClientDataAction cloud             = ClientDataAction::None;

    CloudColorMode cloudColorMode       = CloudColorMode::FromDepthSizedColorImage;

    [[nodiscard]] constexpr auto capture_color()                const noexcept -> bool{return do_process(color);}
    [[nodiscard]] constexpr auto capture_depth()                const noexcept -> bool{return do_process(depth);}
    [[nodiscard]] constexpr auto capture_infra()                const noexcept -> bool{return do_process(infra);}
    [[nodiscard]] constexpr auto capture_bodies_id_map()        const noexcept -> bool{return do_process(bodiesIdMap);}
    [[nodiscard]] constexpr auto capture_audio()                const noexcept -> bool{return do_process(audio);}
    [[nodiscard]] constexpr auto capture_imu()                  const noexcept -> bool{return do_process(IMU);}
    [[nodiscard]] constexpr auto capture_body_tracking()        const noexcept -> bool{return do_process(bodyTracking);}
    [[nodiscard]] constexpr auto generate_depth_size_color()    const noexcept -> bool{return do_process(depthSizedColor);}
    [[nodiscard]] constexpr auto generate_cloud()               const noexcept -> bool{return do_process(cloud);}

    [[nodiscard]] constexpr auto has_data_to_send() const noexcept -> bool{
        return  do_send(color)        || do_send(depth)             || do_send(infra) ||
                do_send(bodiesIdMap)  || do_send(audio)             || do_send(IMU)   ||
                do_send(bodyTracking) || do_send(depthSizedColor)   || do_send(cloud);
    }

    [[nodiscard]] constexpr auto has_data_to_display() const noexcept -> bool{
        return  do_display(color)         || do_display(depth)             || do_display(infra) ||
                do_display(bodiesIdMap)   || do_display(depthSizedColor)   || do_display(cloud);
    }
};

struct DCServerDataActions{
    ServerDataAction color            = ServerDataAction::None;
    ServerDataAction depthSizedColor  = ServerDataAction::Process;
    ServerDataAction bodiesIdMap      = ServerDataAction::None;
    ServerDataAction infra            = ServerDataAction::None;
    ServerDataAction depth            = ServerDataAction::Process;
    ServerDataAction cloud            = ServerDataAction::None;
    ServerDataAction audio            = ServerDataAction::None;
    ServerDataAction IMU              = ServerDataAction::None;
    ServerDataAction bodyTracking     = ServerDataAction::None;

    CloudGenerationMode cloudGenMode  = CloudGenerationMode::FromDepth;
    CloudColorMode cloudColorMode     = CloudColorMode::FromDepthSizedColorImage;
};

struct DCDataSettings : io::BinaryFileSettings{

    DCClientDataActions client;
    DCServerDataActions server;

    DCDataSettings();
    DCDataSettings(std::int8_t const * const data, size_t &offset, size_t sizeData){
        DCDataSettings::init_from_data(data, offset, sizeData);
    }

    // i/o
    auto init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void override;
    auto write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void override;
    auto total_data_size() const noexcept-> size_t override;
};
}
