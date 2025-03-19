// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Associated header
//
#include "k4aaudiomanager.h"

// Library headers
//
#include <libusb.h>

// Project headers
//
#include "k4adevicecorrelator.h"

using namespace k4a;

struct Global{
    static inline std::unique_ptr<K4AAudioManager> instance = nullptr;
};
auto K4AAudioManager::get_instance() -> K4AAudioManager*{
    if(Global::instance == nullptr){
        Global::instance = std::make_unique<K4AAudioManager>();
    }
    return Global::instance.get();
}

auto K4AAudioManager::initialize(SoundIoBackend backend) -> int{
    return initialize_impl([backend](SoundIo *soundIo) {
        return soundio_connect_backend(soundIo, backend);
    });
}

auto K4AAudioManager::initialize() -> int{
    return initialize_impl(soundio_connect);
}

auto K4AAudioManager::initialize_impl(const std::function<int(SoundIo *)> &initFn) -> int{

    m_io.reset(soundio_create());
    m_initStatus = initFn(m_io.get());

    if (m_initStatus != SoundIoErrorNone){
        return m_initStatus;
    }

    return refresh_devices();
}

auto K4AAudioManager::refresh_devices() -> int{

    if (!m_io){
        return SoundIoErrorInvalid;
    }

    soundio_flush_events(m_io.get());

    m_devicesNames.clear();
    m_inputDevices.clear();

    const int inputCount = soundio_input_device_count(m_io.get());
    for (int i = 0; i < inputCount; i++){
        std::shared_ptr<SoundIoDevice> device(soundio_get_input_device(m_io.get(), i), SoundIoDeviceDeleter());
        if (device){
            // Each device is listed twice - a 'raw' device and a not-'raw' device.
            // We only want the non-raw ones.
            //
            if (device->is_raw){
                continue;
            }

            m_devicesNames.push_back(device->name);
            m_inputDevices[device->name] = std::move(device);
        }
    }

    return SoundIoErrorNone;
}

auto K4AAudioManager::get_microphone_for_device(const std::string &deviceName) -> std::shared_ptr<K4AMicrophone>{
    const auto soundIoDevice = m_inputDevices.find(deviceName);
    if (soundIoDevice == m_inputDevices.end()){
        return nullptr;
    }
    return std::shared_ptr<K4AMicrophone>(new K4AMicrophone(soundIoDevice->second));
}

