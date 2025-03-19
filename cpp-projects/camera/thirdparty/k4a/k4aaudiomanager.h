// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// std
#include <functional>
#include <map>
#include <memory>

// k4a
#include "k4asoundio_util.h"
#include "k4amicrophone.h"

namespace k4a{

class K4AAudioManager{
public:

    K4AAudioManager() = default;
    ~K4AAudioManager() = default;

    static auto get_instance() -> K4AAudioManager*;

    auto initialize(SoundIoBackend backend) -> int;
    auto initialize() -> int;
    auto is_initialized() -> bool{
        return m_initStatus == SoundIoErrorNone;
    }
    auto refresh_devices() -> int;

    auto get_devices_count() const -> size_t{
        return m_devicesNames.size();
    }

    auto get_device_name(size_t id) const -> std::string{
        if(id < m_devicesNames.size()){
            return m_devicesNames[id];
        }
        return "";
    }

    auto get_microphone_for_device(const std::string &deviceName) -> std::shared_ptr<K4AMicrophone>;

private:    

    auto initialize_impl(const std::function<int(SoundIo *)> &initFn) -> int;

    SoundIoUniquePtr m_io;
    std::vector<std::string> m_devicesNames;
    std::map<std::string, std::shared_ptr<SoundIoDevice>> m_inputDevices;

    int m_initStatus = -1;
};
}

