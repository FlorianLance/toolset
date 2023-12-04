
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


#include "k4_device.hpp"

// std
#include <thread>
#include <execution>
#include <iostream>

// libyuv
#include "thirdparty/libyuv/libyuv.h"

// local
// # kinect4
#include "k4a/k4astaticimageproperties.h"
#include "k4a/k4amicrophonelistener.h"
#include "k4a/k4aaudiomanager.h"
#include "kabt/k4abt.hpp"
// # utility
#include "utility/time.hpp"
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"
#include "utility/vector.hpp"
#include "utility/string_unordered_map.hpp"
//#include "utility/format.hpp"
// # geometry
#include "geometry/point4.hpp"
// # graphics
#include "graphics/color.hpp"
// # camera
#include "k4_types.hpp"
#include "camera/dc_frame_compressor.hpp"
#include "camera/dc_frame_uncompressor.hpp"
#include "k4_compressed_frame.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::camera;


struct K4Device::Impl{

    enum class Connectivity : std::int8_t{
        Connectivity_4,
        Connectivity_8
    };

    K4Device *kinect4 = nullptr;

    // device
    uint32_t deviceCount = 0;
    std::string serialNumber;
    k4a::device device;
    k4a::calibration calibration;    
    k4a::transformation transformation;    
    k4abt::tracker tracker;
    k4a_device_configuration_t k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    k4abt_tracker_configuration_t k4aBtConfig = K4ABT_TRACKER_CONFIG_DEFAULT;

    DCConfigSettings config;
    std::unique_ptr<k4a::capture> capture = nullptr;

    // images
    // # timestamps
    std::chrono::nanoseconds colorImageTS = std::chrono::nanoseconds{0};
    std::chrono::nanoseconds depthImageTS = std::chrono::nanoseconds{0};
    std::chrono::nanoseconds infraredImageTS = std::chrono::nanoseconds{0};
    // # capture    
    std::optional<k4a::image> colorImage         = std::nullopt;
    std::optional<k4a::image> depthImage         = std::nullopt;
    std::optional<k4a::image> infraredImage      = std::nullopt;
    std::optional<k4a::image> pointCloudImage    = std::nullopt;
    // # processing
    std::optional<k4a::image> convertedColorImage = std::nullopt;
    std::optional<k4a::image> depthSizedColorImage = std::nullopt;

    // audio
    std::shared_ptr<k4a::K4AMicrophone> microphone = nullptr;
    std::shared_ptr<k4a::K4AMicrophoneListener> audioListener = nullptr;
    size_t lastFrameCount = 0;
    std::vector<k4a::K4AMicrophoneFrame> audioFrames;

    // imu
    DCImuSample imuSample;

    // bodies
    std::chrono::nanoseconds bodiesTS = std::chrono::nanoseconds{0};
    size_t bodiesCount = 0;
    std::vector<K4Body> bodies;

    // parameters
    DCDataSettings data;
    DCFilters filters;

    // infos
    size_t idCapture    = 0;
    size_t colorWidth   = 0;
    size_t colorHeight  = 0;
    size_t colorSize    = 0;
    size_t depthWidth   = 0;
    size_t depthHeight  = 0;
    size_t depthSize    = 0;
    DCColorResolution colorResolution;
    DCImageFormat imageFormat;
    DCDepthMode depthMode;

    // profiling
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> timestamps;
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> localTimestamps;
    float nbCapturePerSecond = 0.f;

    // state
    bool camerasStarted = false;
    std::atomic_bool readFramesFromCameras = false;
    std::atomic_bool sendData = true;

    float temperature = 0.f;
    size_t validDepthValues = 0;
    size_t meanBiggestZoneId = 0;

    // arrays indices
    std::vector<size_t> indicesDepths1D;
    std::vector<size_t> indicesDepths1DNoBorders;
    std::vector<std::array<std::int32_t,4>> indicesNeighbours4Depth1D;
    std::vector<std::array<std::int32_t,8>> indicesNeighbours8Depth1D;    
    std::vector<std::tuple<size_t, std::int32_t>> indexDepthVertexCorrrespondance;

    std::vector<std::tuple<size_t, std::int16_t>> indicesDepthsSortedCorrespondanceNoBorders;

    std::vector<geo::Pt3<size_t>> indicesDepths3D;
    std::vector<size_t> indicesColors1D;

    std::vector<std::int8_t> filteringMask;
    std::vector<int> depthMask;
    std::vector<int> zonesId;

    std::vector<std::int16_t> depthFiltering;

    // compression
    DCFrameCompressor frameCompressor;
    DCFrameUncompressor frameUncompressor;

    // thread/lockers
    std::mutex parametersM; /**< mutex for reading parameters at beginning of a new frame in thread function */
    std::unique_ptr<std::thread> frameReaderT = nullptr;

    // delay buffer
    std::int64_t millisecondsDelay = 0;
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<DCFrame>>> frames;
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<camera::K4CompressedFrame>>> compressedFrames;

    // functions
    auto read_frames(DCMode mode) -> void;
    // # init data
    auto init_data(DCMode mode) -> void;
    // # read data
    auto read_from_microphones() -> void;
    auto read_from_imu() -> void;
    // # get images
    auto get_color_image() -> bool;
    auto get_depth_image() -> bool;
    auto get_infra_image(DCMode mode) -> bool;
    // # processing
    auto convert_color_image(const DCFilters &f) -> void;
    auto resize_color_to_fit_depth() -> void;
    auto filter_depth_image(const DCFilters &f, DCMode mode) -> void;
    auto filter_color_image(const DCFilters &f) -> void;
    auto filter_infrared_image(const DCFilters &f) -> void;
    auto generate_cloud(const DCDataSettings &d, DCMode mode) -> void;
    auto compress_frame(const DCFilters &f, const DCDataSettings &d, DCMode mode) -> std::unique_ptr<K4CompressedFrame>;
    auto create_local_frame(const DCDataSettings &d, DCMode mode) -> std::unique_ptr<DCFrame>;

    // profiling
    auto get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::milliseconds>;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::microseconds>;


    static auto generate_config(bool synchInConnected, bool synchOutConnected, const DCConfigSettings &config) -> k4a_device_configuration_t;
    static auto generate_config(
        bool synchInConnected,
        bool synchOutConnected,
        DCImageFormat colFormat,
        DCColorResolution colResolution,
        DCDepthMode depthMode = DCDepthMode::K4_NFOV_UNBINNED,
        DCFramerate fps = DCFramerate::F30,
        bool synchronizeColorAndDepth = true,
        int delayBetweenColorAndDepthUsec = 0,
        DCSynchronisationMode synchMode = DCSynchronisationMode::K4_Standalone,
        int subordinateDelayUsec = 0,
        bool disableLED = false) -> k4a_device_configuration_t;

    static auto generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t;

private:

    auto maximum_local_depth_difference(float max, Connectivity connectivity) -> void;
    auto keep_only_biggest_cluster() -> void;
    auto mininum_neighbours(std::uint8_t nbLoops, std::uint8_t nbMinNeighbours, Connectivity connectivity) -> void;
    auto erode(std::uint8_t nbLoops, Connectivity connectivity) -> void;
};

auto K4Device::Impl::get_duration_between_ms(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::milliseconds>{

    std::unique_lock<std::mutex> lock(parametersM, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }

    if(timestamps.contains(from) && timestamps.contains(to)){
        if(timestamps.at(from).has_value() && timestamps.at(to).has_value()){
            return std::chrono::duration_cast<std::chrono::milliseconds>(timestamps.at(to).value()-timestamps.at(from).value());
        }
        return std::nullopt;
    }
    return std::nullopt;
}

auto K4Device::Impl::get_duration_between_micro_s(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::microseconds>{

    std::unique_lock<std::mutex> lock(parametersM, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }

    if(timestamps.contains(from) && timestamps.contains(to)){
        if(timestamps.at(from).has_value() && timestamps.at(to).has_value()){
            return std::chrono::duration_cast<std::chrono::microseconds>(timestamps.at(to).value()-timestamps.at(from).value());
        }
        return std::nullopt;
    }
    return std::nullopt;
}

auto K4Device::Impl::generate_config(bool synchInConnected, bool synchOutConnected,const DCConfigSettings &config) -> k4a_device_configuration_t{

    k4a_device_configuration_t ka4Config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    ka4Config.color_format               = static_cast<k4a_image_format_t>(get_k4_image_format(image_format(config.mode)));
    ka4Config.color_resolution           = static_cast<k4a_color_resolution_t>(color_resolution(config.mode));
    ka4Config.depth_mode                 = static_cast<k4a_depth_mode_t>(depth_mode(config.mode));
    ka4Config.camera_fps                 = static_cast<k4a_fps_t>(framerate(config.mode));

    ka4Config.synchronized_images_only = false;

    if(depth_mode(config.mode) == DCDepthMode::K4_OFF){
        ka4Config.synchronized_images_only = false;
    }else{
        ka4Config.synchronized_images_only = config.synchronizeColorAndDepth;
    }
    ka4Config.depth_delay_off_color_usec        = config.delayBetweenColorAndDepthUsec;
    ka4Config.disable_streaming_indicator       = config.disableLED;
    ka4Config.wired_sync_mode = static_cast<k4a_wired_sync_mode_t>(config.synchMode);
    ka4Config.subordinate_delay_off_master_usec = config.subordinateDelayUsec;

    // check modes
    if(config.synchMode == DCSynchronisationMode::K4_Subordinate){
        if(!synchInConnected){
            ka4Config.wired_sync_mode = static_cast<k4a_wired_sync_mode_t>(DCSynchronisationMode::K4_Standalone);
            Logger::warning("No input synchronisation cable found, switch from [Subordinate] to [Standalone] mode and set subordinate delay to [0].\n");
        }
    }else if(config.synchMode == DCSynchronisationMode::K4_Master){
        if(!synchOutConnected){
            ka4Config.wired_sync_mode = static_cast<k4a_wired_sync_mode_t>(DCSynchronisationMode::K4_Standalone);
            Logger::warning("No output synchronisation cable found, switch from [Master] to [Standalone] mode.\n");
        }
    }

    if(config.synchMode == DCSynchronisationMode::K4_Master && config.subordinateDelayUsec != 0){
        Logger::warning("Subordinate delay != 0 for mode [Master], subordinate delay is now set to [0].\n");
        ka4Config.subordinate_delay_off_master_usec = 0;
    }else if (config.synchMode == DCSynchronisationMode::K4_Standalone && config.subordinateDelayUsec != 0){
        Logger::warning("Subordinate delay != 0 for mode [Standalone], subordinate delay is now set to [0].\n");
        ka4Config.subordinate_delay_off_master_usec = 0;
    }

    return ka4Config;
}

auto K4Device::Impl::generate_config(
    bool synchInConnected,
    bool synchOutConnected,
    DCImageFormat colFormat,
    DCColorResolution colResolution,
    DCDepthMode depthMode,
    DCFramerate fps,
    bool synchronizeColorAndDepth,
    int delayBetweenColorAndDepthUsec,
    DCSynchronisationMode synchMode,
    int subordinateDelayUsec,
    bool disableLED) -> k4a_device_configuration_t{

    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.color_format                      = static_cast<k4a_image_format_t>(get_k4_image_format(colFormat));
    config.color_resolution                  = static_cast<k4a_color_resolution_t>(colResolution);
    config.depth_mode                        = static_cast<k4a_depth_mode_t>(depthMode);
    config.camera_fps                        = static_cast<k4a_fps_t>(fps);

    if(depthMode == DCDepthMode::K4_OFF){
        config.synchronized_images_only = false;
    }else{
        config.synchronized_images_only = synchronizeColorAndDepth;
    }
    config.depth_delay_off_color_usec        = delayBetweenColorAndDepthUsec;
    config.disable_streaming_indicator       = disableLED;

    // check modes
    if(synchMode == DCSynchronisationMode::K4_Subordinate){
        if(!synchInConnected){
            synchMode = DCSynchronisationMode::K4_Standalone;
            Logger::warning("No input synchronisation cable found, switch from [Subordinate] to [Standalone] mode and set subordinate delay to [0].\n");
        }
    }else if(synchMode == DCSynchronisationMode::K4_Master){
        if(!synchOutConnected){
            synchMode = DCSynchronisationMode::K4_Standalone;
            Logger::warning("No output synchronisation cable found, switch from [Master] to [Standalone] mode.\n");
        }
    }

    if(synchMode == DCSynchronisationMode::K4_Master && subordinateDelayUsec != 0){
        Logger::warning("Subordinate delay != 0 for mode [Master], subordinate delay is now set to [0].\n");
        subordinateDelayUsec = 0;
    }else if (synchMode == DCSynchronisationMode::K4_Standalone && subordinateDelayUsec != 0){
        Logger::warning("Subordinate delay != 0 for mode [Standalone], subordinate delay is now set to [0].\n");
        subordinateDelayUsec = 0;
    }
    config.wired_sync_mode                   = static_cast<k4a_wired_sync_mode_t>(synchMode);;
    config.subordinate_delay_off_master_usec = subordinateDelayUsec;

    Logger::message(std::format("config.color_format: {}\n", static_cast<int>(config.color_format)));
    Logger::message(std::format("config.color_resolution: {}\n", static_cast<int>(config.color_resolution)));
    Logger::message(std::format("config.depth_mode: {}\n", static_cast<int>(config.depth_mode)));
    Logger::message(std::format("config.camera_fps: {}\n", static_cast<int>(config.camera_fps)));

    return config;
}

auto K4Device::Impl::generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t{

    k4abt_tracker_configuration_t ka4BtConfig;
    ka4BtConfig.gpu_device_id       = config.btGPUId;
    ka4BtConfig.processing_mode     = static_cast<k4abt_tracker_processing_mode_t>(config.btProcessingMode);
    ka4BtConfig.sensor_orientation  = static_cast<k4abt_sensor_orientation_t>(config.btOrientation);
    ka4BtConfig.model_path          = nullptr;
    return ka4BtConfig;
}




K4Device::K4Device() : i(std::make_unique<Impl>()){

    i->kinect4 = this;

    // init audio manager
    auto audioM = k4a::K4AAudioManager::get_instance();
    if(!audioM->is_initialized()){
        const int audioInitStatus = audioM->initialize();
        if (audioInitStatus != SoundIoErrorNone){
            Logger::error("[K4Device] Failed to initialize audio backend: {}\n", soundio_strerror(audioInitStatus));
        }
    }

    refresh_devices_list();
}

K4Device::~K4Device(){
    clean();
}

auto K4Device::refresh_devices_list() -> void{

    // cameras
    i->deviceCount = k4a::device::get_installed_count();
    if(i->deviceCount == 0){
        Logger::error("[K4Device] No K4A devices found\n");
    }else{
        Logger::message(std::format("[K4Device] Devices found: {}\n", i->deviceCount));
    }

    // microphones
    if(i->microphone != nullptr){
        if(i->microphone->IsStarted()){
            i->microphone->Stop();
        }
        i->microphone = nullptr;
    }


    auto audioM = k4a::K4AAudioManager::get_instance();
    audioM->refresh_devices();

    size_t nbDevices = audioM->get_devices_count();
    Logger::message(std::format("[K4Device] Audio devices count: {}\n", nbDevices));

    for(size_t ii = 0; ii < nbDevices; ++ii){
        std::string deviceName = audioM->get_device_name(ii);
        Logger::message(std::format(" - {}\n", deviceName));
        if (deviceName.find("Azure Kinect Microphone Array") != std::string::npos) {
            Logger::message(std::format("Found Azure kinect microphones array.\n"));

            i->microphone = audioM->get_microphone_for_device(deviceName);
            if(i->microphone == nullptr){
                Logger::error(std::format("[K4Device] Cannot retrieve microphone.\n"));
                i->audioListener = nullptr;
                return;
            }

            Logger::message("[K4Device] Start microphone. \n");
            if(i->microphone->Start() != SoundIoErrorNone){
                Logger::error(std::format("[K4Device] Cannot start microphone.\n"));
                return;
            }

            if(i->microphone->IsStarted()){
                Logger::message("[K4Device] Create listener. \n");
                i->audioListener = i->microphone->CreateListener();
                if(i->audioListener == nullptr){
                    Logger::error(std::format("[K4Device] Cannot init audio listener.\n"));
                    return;
                }
            }
        }
    }

    if(i->audioListener != nullptr){
        Logger::message("[K4Device] Microphone listener created. \n");
    }

}

auto K4Device::open(std::uint32_t deviceId) -> bool{

    if(is_opened()){
        return false;
    }

    i->serialNumber  = "";

    if(deviceId >= i->deviceCount){
        Logger::error("[K4Device] Invalid device id\n");
        return false;
    }

    try {
        i->device = k4a::device::open(i->config.idDevice = deviceId);
    }  catch (std::runtime_error error) {
        Logger::error(std::format("[K4Device] open error: {}\n", error.what()));
        return false;
    }

    const auto version = i->device.get_version();
    const auto fb = version.firmware_build;
    const auto fs = version.firmware_signature;
    bool debugFB =  fb == K4A_FIRMWARE_BUILD_DEBUG;
    std::string fsStr;
    switch (fs) {
    case K4A_FIRMWARE_SIGNATURE_MSFT:
        fsStr = "Microsoft signed";
        break;
    case K4A_FIRMWARE_SIGNATURE_TEST:
        fsStr = "Test signed";
        break;
    case K4A_FIRMWARE_SIGNATURE_UNSIGNED:
        fsStr = "Unsigned";
        break;
    }

    i->serialNumber = i->device.get_serialnum();

    Logger::message("[K4Device] Device opened:\n");
    Logger::message(std::format("  Serialnum: {}\n", i->serialNumber));
    Logger::message("  Version:\n");
    Logger::message(std::format("      Firmware build: {}\n", (debugFB ? "[debug]" : "[release]")));
    Logger::message(std::format("      Firmware signature: {}\n", fsStr));
    Logger::message(std::format("      Color camera firmware version {}.{}.{}\n", version.rgb.major, version.rgb.minor, version.rgb.iteration));
    Logger::message(std::format("      Depth camera firmware version {}.{}.{}\n", version.depth.major, version.depth.minor, version.depth.iteration));
    Logger::message(std::format("      Audio device firmware version {}.{}.{}\n", version.audio.major, version.audio.minor, version.audio.iteration));
    Logger::message(std::format("      Depth device firmware version {}.{}.{}\n", version.depth_sensor.major, version.depth_sensor.minor, version.depth_sensor.iteration));
    Logger::message("  Synch:\n");
    Logger::message(std::format("      IN connected {}\n", i->device.is_sync_in_connected()));
    Logger::message(std::format("      OUT connected {}\n", i->device.is_sync_out_connected()));


    return true;
}

auto K4Device::nb_devices() const noexcept -> uint32_t{
    return i->deviceCount;
}

auto K4Device::device_name() const noexcept-> std::string{
    return i->serialNumber;
}

auto K4Device::is_opened() const noexcept -> bool{
    return i->device.is_valid();
}

auto K4Device::cameras_started() const noexcept -> bool{
    return i->camerasStarted;
}

auto K4Device::is_sync_in_connected() const noexcept -> bool{
    return i->device.is_sync_in_connected();
}

auto K4Device::is_sync_out_connected() const noexcept -> bool{
    return i->device.is_sync_out_connected();
}

auto K4Device::get_nb_capture_per_second() const noexcept -> float {
    return i->nbCapturePerSecond;
}

auto K4Device::mode() const noexcept -> DCMode{
    return i->config.mode;
}

auto K4Device::get_capture_duration_ms() noexcept -> int64_t{
    if(auto duration = i->get_duration_between_ms("before_capture"sv, "after_capture"sv); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto K4Device::get_processing_duration_ms() noexcept -> int64_t{
    if(auto duration = i->get_duration_between_ms("after_capture"sv, "after_processing"sv); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto K4Device::get_compressing_duration_ms() noexcept -> int64_t{
    if(auto duration = i->get_duration_between_ms("after_processing"sv, "after_capture"sv); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto K4Device::get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> int64_t{
    if(auto duration = i->get_duration_between_ms(from, to); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto K4Device::get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> int64_t{
    if(auto duration = i->get_duration_between_micro_s(from, to); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}



auto K4Device::is_LED_disabled() const noexcept -> bool{
    return i->config.disableLED;
}

auto K4Device::synch_mode() const noexcept -> DCSynchronisationMode{
    return i->config.synchMode;
}

auto K4Device::subordinate_delay_usec() const noexcept -> int{
    return i->config.subordinateDelayUsec;
}

auto K4Device::color_and_depth_synchronized() const noexcept -> bool{
    return i->config.synchronizeColorAndDepth;
}

auto K4Device::delay_between_color_and_depth_usec() const noexcept -> int{
    return i->config.delayBetweenColorAndDepthUsec;
}

auto K4Device::close() -> void{

    if(i->microphone){
        if(i->microphone->IsStarted()){
            i->microphone->Stop();
        }
        if(i->audioListener){
            i->audioListener = nullptr;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if(i->readFramesFromCameras){
        Logger::error("[K4Device] Reading must be stopped before closing the device.\n");
        return;
    }
    i->device.close();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}


auto K4Device::clean() -> void{
    if(is_opened()){
        stop_cameras();
        close();
    }
}

auto K4Device::device_id() const noexcept -> std::uint32_t{
    return i->config.idDevice;
}


auto K4Device::set_data_settings(const DCDataSettings &dataS) -> void {
    i->parametersM.lock();
    i->data = dataS;
//    i->tracker.set_temporal_smoothing(dataS.btTemporalSmoothing);
    i->parametersM.unlock();
}

auto K4Device::set_filters(const DCFilters &filters) -> void{
    i->parametersM.lock();
    i->filters = filters;
    i->parametersM.unlock();
}

void K4Device::set_color_settings(const DCColorSettings &colorS) {

    k4a_color_control_mode_t mode;
    std::int32_t currentValue, newValue;
    k4a_color_control_command_t type;

    try{

        type = K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE;
        i->device.get_color_control(type, &mode, &currentValue);

        switch(static_cast<K4ExposureTimesMicroS>(colorS.exposureTimeAbsolute)){
            case K4ExposureTimesMicroS::t500: newValue = 500; break;
            case K4ExposureTimesMicroS::t1250: newValue = 1250; break;
            case K4ExposureTimesMicroS::t2500: newValue = 2500; break;
            case K4ExposureTimesMicroS::t8330: newValue = 8330; break;
            case K4ExposureTimesMicroS::t16670: newValue = 16670; break;
            case K4ExposureTimesMicroS::t33330:
            if(colorS.powerlineFrequency == PowerlineFrequency::F60){
                newValue = 33330;
            }else{
                newValue = 30000;
            }break;
        }

        if((currentValue != newValue) || (mode != (colorS.autoExposureTime ? K4A_COLOR_CONTROL_MODE_AUTO : K4A_COLOR_CONTROL_MODE_MANUAL))){
            i->device.set_color_control(
                type,
                colorS.autoExposureTime ? K4A_COLOR_CONTROL_MODE_AUTO : K4A_COLOR_CONTROL_MODE_MANUAL,
                newValue
            );
        }

        type = K4A_COLOR_CONTROL_WHITEBALANCE;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = colorS.whiteBalance - (colorS.whiteBalance % 10);

        if((currentValue != newValue) || (mode != (colorS.autoWhiteBalance ? K4A_COLOR_CONTROL_MODE_AUTO : K4A_COLOR_CONTROL_MODE_MANUAL))){
            i->device.set_color_control(
                type,
                colorS.autoWhiteBalance ? K4A_COLOR_CONTROL_MODE_AUTO : K4A_COLOR_CONTROL_MODE_MANUAL,
                newValue
            );
        }

        type = K4A_COLOR_CONTROL_BRIGHTNESS;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = static_cast<std::int32_t>(colorS.brightness);
        if(currentValue != newValue){
            i->device.set_color_control(type, K4A_COLOR_CONTROL_MODE_MANUAL, newValue);
        }

        type = K4A_COLOR_CONTROL_CONTRAST;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = static_cast<std::int32_t>(colorS.contrast);
        if(currentValue != newValue){
            i->device.set_color_control(type, K4A_COLOR_CONTROL_MODE_MANUAL, newValue);
        }

        type = K4A_COLOR_CONTROL_SHARPNESS;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = static_cast<std::int32_t>(colorS.sharpness);
        if(currentValue != newValue){
            i->device.set_color_control(type, K4A_COLOR_CONTROL_MODE_MANUAL, newValue);
        }

        type = K4A_COLOR_CONTROL_SATURATION;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = static_cast<std::int32_t>(colorS.saturation);
        if(currentValue != newValue){
            i->device.set_color_control(type, K4A_COLOR_CONTROL_MODE_MANUAL, newValue);
        }

        type = K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = colorS.backlightCompensation ? 1 : 0;
        if(currentValue != newValue){
            i->device.set_color_control(type, K4A_COLOR_CONTROL_MODE_MANUAL, newValue);
        }

        type = K4A_COLOR_CONTROL_GAIN;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = static_cast<std::int32_t>(colorS.gain);
        if(currentValue != newValue){
            i->device.set_color_control(type, K4A_COLOR_CONTROL_MODE_MANUAL, newValue);
        }

        type = K4A_COLOR_CONTROL_POWERLINE_FREQUENCY;
        i->device.get_color_control(type, &mode, &currentValue);
        newValue = static_cast<std::int32_t>(colorS.powerlineFrequency);
        if(currentValue != newValue){
            i->device.set_color_control(type, K4A_COLOR_CONTROL_MODE_MANUAL, newValue);
        }

    }  catch (std::runtime_error error) {
        Logger::error(std::format("[K4Device] Set color settings error: {} T:{} CV:{} NV:{}\n", error.what(), static_cast<int>(type), currentValue, newValue));
    }
}

auto K4Device::send_data_state(bool state) -> void {
    i->sendData = state;
}

auto K4Device::set_delay(DCDelaySettings delay) -> void{
    i->millisecondsDelay = delay.delayMs;
}

auto K4Device::start_cameras(const DCConfigSettings &configS) -> bool{

    if(cameras_started() || !is_opened()){
        return false;
    }

    i->config      = configS;
    i->k4aConfig   = i->generate_config(i->device.is_sync_in_connected(), i->device.is_sync_out_connected(), i->config);
    i->k4aBtConfig = i->generate_bt_config(i->config);

    try {

        Logger::message("[K4Device] start cameras\n");        
        Logger::message("[Config]\n");
        Logger::message(std::format("   color format: {}\n", static_cast<int>(i->k4aConfig.color_format)));
        Logger::message(std::format("   depth mode: {}\n", static_cast<int>(i->k4aConfig.depth_mode)));
        i->device.start_cameras(&i->k4aConfig);

        Logger::message("[K4Device] Retrieve calibration\n");
        i->calibration = i->device.get_calibration(i->k4aConfig.depth_mode, i->k4aConfig.color_resolution);

        const auto &c = i->calibration;
        Logger::message("[Calibration]\n");
        Logger::message(std::format("  color resolution: {}\n", static_cast<int>(c.color_resolution)));
        Logger::message("  color camera:\n");
        Logger::message(std::format("      width: {}\n", c.color_camera_calibration.resolution_width));
        Logger::message(std::format("      height: {}\n", c.color_camera_calibration.resolution_height));
        Logger::message(std::format("      metric radius: {}\n", c.color_camera_calibration.metric_radius));
        Logger::message("  depth mode:\n");
        Logger::message(std::format("      width: {}\n", c.depth_camera_calibration.resolution_width));
        Logger::message(std::format("      height: {}\n", c.depth_camera_calibration.resolution_height));  

        Logger::message("[K4Device] start imu\n");
        i->device.start_imu();

        if(depth_mode(configS.mode) != DCDepthMode::K4_OFF){
            Logger::message("[K4Device] start body tracker\n");
//            i->tracker = k4abt::tracker::create(i->calibration, i->k4aBtConfig);
        }

    }  catch (k4a::error error) {
        Logger::error("[K4Device] start_cameras error: {}\n", error.what());
        i->k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }  catch (std::runtime_error error) {
        Logger::error("[K4Device] start_cameras error: {}\n", error.what());
        i->k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }

    Logger::message("[K4Device] Color control\n");
    k4a_color_control_mode_t mode;
    std::int32_t value;
    i->device.get_color_control(K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, &mode, &value);
    Logger::message(std::format("      Exposure time: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_BRIGHTNESS, &mode, &value);
    Logger::message(std::format("      Brightness: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_CONTRAST, &mode, &value);
    Logger::message(std::format("      Contrast: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_SHARPNESS, &mode, &value);
    Logger::message(std::format("      Sharpness: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_SATURATION, &mode, &value);
    Logger::message(std::format("      Saturation: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_WHITEBALANCE, &mode, &value);
    Logger::message(std::format("      White balance: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION, &mode, &value);
    Logger::message(std::format("      Backlight compensation: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_GAIN, &mode, &value);
    Logger::message(std::format("      Gain: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));
    i->device.get_color_control(K4A_COLOR_CONTROL_POWERLINE_FREQUENCY, &mode, &value);
    Logger::message(std::format("      Powerline frequency: [{}] [{}]\n", value, mode == K4A_COLOR_CONTROL_MODE_AUTO ? "auto" : "manual"));

    Logger::message("[K4Device] Camera started\n");
    i->camerasStarted = true;

    // start thread
    i->frameReaderT = std::make_unique<std::thread>(&K4Device::Impl::read_frames, i.get(), i->config.mode);

    return true;
}



auto K4Device::stop_cameras() -> void{

    i->readFramesFromCameras = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if(i->frameReaderT != nullptr){
        if(i->frameReaderT->joinable()){
            i->frameReaderT->join();
        }
        i->frameReaderT = nullptr;
    }

    if(i->camerasStarted){
        if(depth_mode(i->config.mode) != DCDepthMode::K4_OFF){
            Logger::message("[K4Device] Shutdown body tracker\n");
            i->tracker.shutdown();
        }
        Logger::message("[K4Device] Stop IMU\n");
        i->device.stop_imu();
        Logger::message("[K4Device] Stop cameras\n");
        i->device.stop_cameras();        
        i->camerasStarted = false;        
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

auto K4Device::Impl::read_frames(DCMode mode) -> void{

    // check device
    if(!device.is_valid() || readFramesFromCameras){
        Logger::error("[K4Device::Impl::read_frames] Cannot start reading frames.\n");
        return;
    }

    // initialization
    init_data(mode);

    // start loop
    readFramesFromCameras = true;

    auto fps = framerate(mode);
    int32_t timeoutMs = 0;
    switch (fps) {
    case DCFramerate::F30:
        timeoutMs = 40;
        break;
    case DCFramerate::F15:
        timeoutMs = 70;
        break;
    case DCFramerate::F5:
        timeoutMs = 250;
        break;
    }

    std::vector<std::chrono::nanoseconds> capturesTimes;    
    timestamps.clear();
    localTimestamps.clear();

    while(readFramesFromCameras){

        // copy parameters
        parametersM.lock();
        const auto f = filters;
        const auto d = data;        
        for(auto &ts : localTimestamps){
            timestamps[ts.first] = ts.second;
            ts.second = std::nullopt;
        }
        parametersM.unlock();

        // read data from device
        try {

            // update before capture timestamp
            localTimestamps["before_capture"sv] = Time::nanoseconds_since_epoch();

            Bench::start("[K4Device::Impl::read_frames] Device get_capture");
            bool success = false;
            try{
                success   = device.get_capture(capture.get(), std::chrono::milliseconds(timeoutMs));
                localTimestamps["after_capture"sv] = Time::nanoseconds_since_epoch();
                Bench::stop();
            }catch(const std::runtime_error &e){
                Logger::error(std::format("[K4Device::Impl::read_frames] Get capture runtime error: {}\n", e.what()));
                Bench::stop();
                readFramesFromCameras = false;
                break;
            }
            if(!success){
                Bench::stop();
            }

            if(d.captureAudio){
                read_from_microphones();
            }

            if(d.captureIMU){
                read_from_imu();
            }

            if(!success){
//                Logger::error(std::format("to[{}] ",timeoutMs));
                continue;
            }

            if(d.captureBodies && (depth_mode(config.mode) != DCDepthMode::K4_OFF)){
                if(tracker.enqueue_capture(*capture.get(), std::chrono::milliseconds(1))){

                    if(k4abt::frame bodyFrame = tracker.pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){
                        bodiesCount = bodyFrame.get_num_bodies();
                        if(bodies.size() < bodiesCount){
                            bodies.resize(bodiesCount);
                        }
                        for(size_t ii = 0; ii < bodiesCount; ++ii){
                            bodies[ii].update(bodyFrame.get_body(ii));
                        }
                        bodiesTS = bodyFrame.get_system_timestamp();
                    }

//                    k4a::image body_index_map = body_frame.get_body_index_map();
//                    if (body_index_map != nullptr){
////                      print_body_index_map_middle_line(body_index_map);
//                    }else{
//                        Logger::error("Error: Failed to generate bodyindex map!\n");
//                    }
                }
            }

            // compute capture framerate
            auto aftertCatpureTs = localTimestamps["after_capture"sv].value();
            capturesTimes.push_back(aftertCatpureTs);
            bool foundT = false;
            size_t idT = 0;
            for(size_t ii = 0; ii < capturesTimes.size(); ++ii){
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(aftertCatpureTs - capturesTimes[ii]);
                if(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() > 5000){
                    foundT = true;
                    idT = ii;
                }else{
                    break;
                }
            }
            if(foundT){
                tool::erase_range(capturesTimes, 0, idT + 1);
            }
            nbCapturePerSecond = capturesTimes.size()/5.f;

        }   catch (std::exception &error) {
            Logger::error(std::format("[K4Device::Impl::read_frames] Get capture error: {}\n", error.what()));
            readFramesFromCameras = false;
            break;
        }

        // get images
        if(!get_color_image()){
            continue;            
        }
        if(!get_depth_image()){
            continue;
        }
        if(!get_infra_image(mode)){
            continue;
        }
        localTimestamps["after_get_images"sv]       = Time::nanoseconds_since_epoch();

//        Logger::message(std::format("{} {} {} {}\n",
//            std::chrono::duration_cast<std::chrono::milliseconds>(colorImageTS),
//            std::chrono::duration_cast<std::chrono::milliseconds>(depthImageTS),
//            std::chrono::duration_cast<std::chrono::milliseconds>(infraredImageTS),
//            std::chrono::duration_cast<std::chrono::milliseconds>(localTimestamps["after_capture"sv].value())
//        ));

        // process
        convert_color_image(f);
        localTimestamps["after_color_convert"sv]    = Time::nanoseconds_since_epoch();

        resize_color_to_fit_depth();
        localTimestamps["after_color_resize"sv]     = Time::nanoseconds_since_epoch();

        filter_depth_image(f, mode);
        localTimestamps["after_depth_filter"sv]     = Time::nanoseconds_since_epoch();

        filter_color_image(f);
        localTimestamps["after_color_filter"sv]     = Time::nanoseconds_since_epoch();

        filter_infrared_image(f);
        localTimestamps["after_infrared_filter"sv]  = Time::nanoseconds_since_epoch();

        generate_cloud(d, mode);
        localTimestamps["after_cloud_generation"sv] = Time::nanoseconds_since_epoch();
        localTimestamps["after_processing"sv]       = Time::nanoseconds_since_epoch();

        if(!readFramesFromCameras){
            break;
        }

        // send frames
        if(sendData){
            if(auto compressedFrame = compress_frame(f,d,mode); compressedFrame != nullptr){

                localTimestamps["after_compressing"sv] = Time::nanoseconds_since_epoch();

                // test
                compressedFrame->afterCaptureTS = localTimestamps["after_compressing"sv].value().count() - compressedFrame->afterCaptureTS;


                // store frame
                compressedFrames.push_back(std::make_tuple(localTimestamps["after_capture"sv].value(), std::move(compressedFrame)));

                // check delay
                using namespace std::chrono;
                bool foundFrame = false;
                size_t idFrame = 0;
                for(size_t ii = 0; ii < compressedFrames.size(); ++ii){
                    auto diff = duration_cast<milliseconds>(localTimestamps["after_capture"sv].value() - std::get<0>(compressedFrames[ii]));
                    if(diff.count() >= millisecondsDelay){
                        foundFrame = true;
                        idFrame = ii;
                    }else{
                        break;
                    }
                }

                // send frame
                if(foundFrame){
                    kinect4->new_compressed_frame_signal(std::move(std::get<1>(compressedFrames[idFrame])));
                    tool::erase_range(compressedFrames, 0, idFrame + 1);
                }
                localTimestamps["after_compressed_frame_sending"sv] = Time::nanoseconds_since_epoch();
            }
        }

        if(d.generateRGBLocalFrame || d.generateDepthLocalFrame || d.generateInfraLocalFrame || d.generateCloudLocal){

            auto dFrame = create_local_frame(d, mode);

            localTimestamps["after_local_frame_creation"sv] = Time::nanoseconds_since_epoch();

            // store frame
            frames.push_back(std::make_tuple(localTimestamps["after_capture"sv].value(), std::move(dFrame)));

            // check delay
            using namespace std::chrono;
            bool foundFrame = false;
            size_t idFrame = 0;
            for(size_t ii = 0; ii < frames.size(); ++ii){
                auto diff = duration_cast<milliseconds>(localTimestamps["after_capture"sv].value() - std::get<0>(frames[ii]));
                if(diff.count() >= millisecondsDelay){
                    foundFrame = true;
                    idFrame = ii;
                }else{
                    break;
                }
            }

            // send frame
            if(foundFrame){
                kinect4->new_frame_signal(std::move(std::get<1>(frames[idFrame])));
                tool::erase_range(frames, 0, idFrame + 1);
            }

        }

        idCapture++;
    }
}


auto K4Device::Impl::filter_depth_image(const DCFilters &f, DCMode mode) -> void{

    if(!depthImage.has_value()){
        return;
    }

    Bench::start("[K4Device] Filter depth");

    // retrieve buffers
    auto depthBuffer = reinterpret_cast<int16_t*>(depthImage->get_buffer());
    ColorRGBA8 *colorBuffer = depthSizedColorImage.has_value() ? reinterpret_cast<ColorRGBA8*>(depthSizedColorImage.value().get_buffer()) : nullptr;
    uint16_t *infraredBuffer = infraredImage.has_value() ? reinterpret_cast<uint16_t*>(infraredImage.value().get_buffer()) : nullptr;

    static_cast<void>(infraredBuffer);

    const auto dRange = range(mode)*1000.f;
    auto minD = f.minDepthValue < dRange.x() ? static_cast<std::int16_t>(dRange.x()) : f.minDepthValue;
    auto maxD = f.maxDepthValue > dRange.y() ? static_cast<std::int16_t>(dRange.y()) : f.maxDepthValue;

    auto hsvDiffColor = Convert::to_hsv(f.filterColor);

    // reset  depth mask
    std::fill(depthMask.begin(), depthMask.end(), 1);

//    for(size_t id = 0; id < indicesDepths1DNoBorders.size(); ++id){
//        indicesDepthsSortedCorrespondanceNoBorders[id] = {indicesDepths1DNoBorders[id], depthBuffer[indicesDepths1DNoBorders[id]]};
//    }
//    auto sortDepth = [](const std::tuple<size_t, std::int16_t> &lhs, const std::tuple<size_t, std::int16_t> &rhs){
//        return std::get<1>(lhs) < std::get<1>(rhs);
//    };
//    std::sort(std::begin(indicesDepthsSortedCorrespondanceNoBorders), std::end(indicesDepthsSortedCorrespondanceNoBorders), sortDepth);

//    auto plane1 = f.plane1;
//    plane1.normal = normalize(plane1.normal);

    // v' = q * v * conjugate(q)
    // t = 2 * cross(q.xyz, v)
    // v' = v + q.w * t + cross(q.xyz, t)

//    auto pl1Tr  = geo::transform(geo::Pt3f{1.f,1.f,1.f}, f.p1Rot, f.p1Pos);
//    auto pl1Dir  = (Vec4f(0,1,0,1)*pl1Tr).xyz();
    auto pl1Dir = normalize(f.p1Rot);//pl1Tr.col(0).xyz();//*-1.f;

//    auto dir = Vec3f{0,1,0};
//    auto q = Quaternion<float>::from_euler(f.p1Rot);
//    auto t = 2.f * cross(Vec3f{q.x,q.y,q.z}, dir);
//    auto pl1Dir = dir + q.w * t + cross(Vec3f{q.x,q.y,q.z}, t);
//    auto pl1Dir = q * Vec3f(1,0,0);
//    auto pl1Dir = normalize(f.p1Rot);

    // depth/width/height/mask/color/infra filtering
    for_each(std::execution::par_unseq, std::begin(indicesDepths3D), std::end(indicesDepths3D), [&](const Pt3<size_t> &dIndex){

        size_t id = dIndex.x();
        size_t ii = dIndex.y();
        size_t jj = dIndex.z();        

        const auto &currentDepth = depthBuffer[id];

        // check validity
        if(currentDepth == dc_invalid_depth_value){
            depthMask[id] = 0;
            return;
        }        

        // depth filtering
        if((ii < f.minWidth)  || (ii > f.maxWidth)  ||      // width
           (jj < f.minHeight) || (jj > f.maxHeight) ||      // height
           (currentDepth < minD) || (currentDepth > maxD) ){// depth
            depthMask[id] = 0;
            return;
        }

        // plane filtering
        if(f.p1FMode != DCFilters::PlaneFilteringMode::None){
            geo::Pt3<float> pt{0.001f * ii,0.001f * jj, 0.001f * currentDepth};

            if(dot(pt - f.p1Pos, pl1Dir) < 0){
                if(f.p1FMode == DCFilters::PlaneFilteringMode::Above){
                    depthMask[id] = 0;
                    return;
                }
            }else{
                if(f.p1FMode == DCFilters::PlaneFilteringMode::Below){
                    depthMask[id] = 0;
                    return;
                }
            }
        }

//        if(plane_equation(pt, f.plane1) < 0){
//            depthMask[id] = 0;
//            return;
//        }

        // mask
//        if(!f.depthMask.at(id)){
//            depthMask[id] = 0;
//            return;
//        }

        // color filtering
        if(depthSizedColorImage.has_value() && f.filterDepthWithColor){

            auto hsv = Convert::to_hsv(colorBuffer[id]);
            if((std::abs(hsv.h()- hsvDiffColor.h()) > f.maxDiffColor.x()) ||
               (std::abs(hsv.s()- hsvDiffColor.s()) > f.maxDiffColor.y()) ||
               (std::abs(hsv.v()- hsvDiffColor.v()) > f.maxDiffColor.z())){
                depthMask[id] = 0;
                return;
            }
        }

        // infrared filtering
        // ...
    });

//    for(size_t ii = 0; ii < 100; ++ii){
//        auto id = std::get<0>(indicesDepthsSortedCorrespondance[ii]);
//        depthMask[id] = 0;
//        std::cout << std::get<1>(indicesDepthsSortedCorrespondance[ii]) << " ";
////        colorBuffer[id].rgba() = {255,0,0,255};
////        std::cout << ii << " " << id << " | ";
//    }

    if(f.doLocalDiffFiltering){
        maximum_local_depth_difference(f.maxLocalDiff, Connectivity::Connectivity_4);
    }

    // minimum neighbours filtering
    if(f.doMinNeighboursFiltering){
        mininum_neighbours(f.minNeighboursLoops, f.nbMinNeighbours, Connectivity::Connectivity_4);
    }

    // erosion
    if(f.doErosion){
        erode(f.erosionLoops, Connectivity::Connectivity_8);
    }

    // remove edges
    if(f.keepOnlyBiggestCluster){// && colorImage.has_value()){

//        std::fill(filteringMask.begin(),filteringMask.end(), 0);

//        for_each(std::execution::seq, std::begin(indicesDepthsSortedCorrespondanceNoBorders), std::end(indicesDepthsSortedCorrespondanceNoBorders), [&](const auto &t){

//            auto id = indicesDepths1DNoBorders[std::get<0>(t)];
//            if(depthMask[id] == 0){
//                return;
//            }

//            size_t count = 0;
//            float currDepth = depthBuffer[id];

//            for(auto cId : indicesNeighbours8Depth1D[id]){

//                if(depthMask[cId] == 0){
//                    continue;
//                }

//                if(currDepth > depthBuffer[cId]){
//                    continue;
//                }

//                if(filteringMask[cId] == 0){
//                    ++count;
//                    continue;
//                }

//                if(std::abs(currDepth - depthBuffer[cId]) > f.maxLocalDiff){
//                    ++count;
//                }
//            }

//            filteringMask[id] = count > f.nbMinNeighbours ? 1 : 0;
//        });

//        for(size_t ii = 0; ii < filteringMask.size(); ++ii){
//            if(filteringMask[ii] == 1){
//                depthMask[ii] = 0;
//            }
//        }



//        // detect edge
//        // si même couleur et depth > 0
//        for(size_t numLoop = 0; numLoop < f.minNeighboursLoops; ++numLoop){

//            std::fill(filteringMask.begin(),filteringMask.end(), 0);

//            for_each(std::execution::par_unseq, std::begin(indicesDepths1DNoBorders), std::end(indicesDepths1DNoBorders), [&](size_t id){

//                if(depthMask[id] == 0){
//                    return;
//                }

//                size_t count = 0;
//                float currDepth = depthBuffer[id];

//                for(auto cId : indicesNeighbours8Depth1D[id]){

//                    if(depthMask[cId] == 0){
//                        continue;
//                    }

////                    if(filteringMask[cId] == 0){
////                        ++count;
////                        continue;
////                    }

//                    if(std::abs(currDepth - depthBuffer[cId]) > f.maxLocalDiff){
//                        ++count;
//                    }
//                }

//                filteringMask[id] = count > f.nbMinNeighbours ? 1 : 0;
//            });

//            for(size_t ii = 0; ii < filteringMask.size(); ++ii){
//                if(filteringMask[ii] == 1){
//                    depthMask[ii] = 0;
//                }
//            }
//        }
    }

    // keep only biggest cluster
    if(f.keepOnlyBiggestCluster){
        keep_only_biggest_cluster();
    }else{
        meanBiggestZoneId = 0;
    }

    // count valid depth values
    validDepthValues = 0;
    for_each(std::execution::unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t id){
        if(depthMask[id] == 0){
            depthBuffer[id] = dc_invalid_depth_value;
//            colorBuffer[id].rgba() = {255,0,0,255};
            indexDepthVertexCorrrespondance[id] = {id, -1};
        }else{
            indexDepthVertexCorrrespondance[id] = {id, validDepthValues};
            validDepthValues++;
        }
    });
    Bench::stop();


    return;



//    // smooth filtering
//    if(f.keepOnlyBiggestCluster){

//        std::fill(depthFiltering.begin(), depthFiltering.end(), k4_invalid_depth_value);

//        float k = 1.f/9.f;

//        for_each(std::execution::par_unseq, std::begin(indicesDepths1DNoBorders), std::end(indicesDepths1DNoBorders), [&](size_t id){

//            if(depthMask[id] == 0){
//                return;
//            }

//            float total = 0.f;
//            for(auto cId : neighbours8Depth1D[id]){
//                if(depthMask[cId] == 1){
//                    total += depthBuffer[cId]*k;
//                }else{
//                    depthFiltering[id] = depthBuffer[id];
//                    return;
//                }
//            }
//            depthFiltering[id] = static_cast<std::int16_t>(total + depthBuffer[id]*k);
//        });

//        std::copy(depthFiltering.begin(), depthFiltering.end(), depthBuffer);
//    }

}

auto K4Device::Impl::keep_only_biggest_cluster() -> void{

    // empty zones
    std::fill(zonesId.begin(), zonesId.end(), 0);

    size_t currentZoneId = 1;
    int biggestZone = -1;
    size_t sizeBiggestZone = 0;
    for_each(std::execution::unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t pt){

        if(zonesId[pt] != 0){
            return;
        }

        if(depthMask[pt] == 0){
            return;
        }

        size_t zoneId = currentZoneId;
        std::queue<size_t> ids;
        ids.push(pt);

        // count new zone
        size_t count = 0;
        while(ids.size() > 0){
            size_t id = ids.front();
            ids.pop();

            if(zonesId[id] != 0){
                continue;
            }

            if(depthMask[id] == 0){
                continue;
            }

            zonesId[id] = zoneId;
            count++;

            if(id >= 1){
                ids.push(id-1);

                if(id >= depthWidth-1){

                    ids.push(id-depthWidth+1);

                    if(id >= depthWidth){

                        ids.push(id-depthWidth);

                        if(id >= depthWidth + 1){
                            ids.push(id-depthWidth-1);
                        }
                    }
                }
            }

            if(id < depthSize-1){

                ids.push(id+1);

                if(id < depthSize -depthWidth+1){
                    ids.push(id+depthWidth-1);

                    if(id < depthSize -depthWidth){
                        ids.push(id+depthWidth);

                        if(id < depthSize -depthWidth-1){
                            ids.push(id+depthWidth+1);
                        }
                    }
                }
            }
        }

        if(count != 0){
            if(count > sizeBiggestZone){
                sizeBiggestZone = count;
                biggestZone = static_cast<int>(currentZoneId);
            }
            currentZoneId++;
        }
    });

    if(biggestZone != -1){
        Pt2<size_t> meanBiggestZone = {0,0};
        size_t count = 0;
        for_each(std::execution::unseq, std::begin(indicesDepths3D), std::end(indicesDepths3D), [&](const auto &pt){
            if(zonesId[pt.x()] != biggestZone){
                depthMask[pt.x()] = 0;
            }

            if(depthMask[pt.x()] == 1){
                meanBiggestZone.x() += pt.y();
                meanBiggestZone.y() += pt.z();
                ++count;
            }
        });

        meanBiggestZone /= count;
        meanBiggestZoneId = meanBiggestZone.y() * depthWidth + meanBiggestZone.x();
    }
}

auto K4Device::Impl::mininum_neighbours(uint8_t nbLoops, uint8_t nbMinNeighbours, Connectivity connectivity) -> void{

    for(std::uint8_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        // reset filtering mask
        std::fill(filteringMask.begin(),filteringMask.end(), 0);

        for_each(std::execution::par_unseq, std::begin(indicesDepths1DNoBorders), std::end(indicesDepths1DNoBorders), [&](size_t id){

            if(depthMask[id] == 0){
                return;
            }

            std::uint8_t count = 0;
            if(connectivity == Connectivity::Connectivity_4){
                for(auto cId : indicesNeighbours4Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : indicesNeighbours8Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 8 ? 1 : 0;
            }

            filteringMask[id] = (count < nbMinNeighbours) ? 1 : 0;
        });

        for(size_t ii = 0; ii < filteringMask.size(); ++ii){
            if(filteringMask[ii] == 1){
                depthMask[ii] = 0;
            }
        }
    }
}

auto K4Device::Impl::maximum_local_depth_difference(float max, Connectivity connectivity) -> void{

    auto depthBuffer = reinterpret_cast<int16_t*>(depthImage->get_buffer());

    std::fill(filteringMask.begin(),filteringMask.end(), 0);

    for_each(std::execution::par_unseq, std::begin(indicesDepths1DNoBorders), std::end(indicesDepths1DNoBorders), [&](size_t id){

        if(depthMask[id] == 0){
            return;
        }

        float meanDiff = 0;
        float currDepth = depthBuffer[id];
        size_t count = 0;
        if(connectivity == Connectivity::Connectivity_4){
            for(auto cId : indicesNeighbours4Depth1D[id]){
                if(depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }else{
            for(auto cId : indicesNeighbours8Depth1D[id]){
                if(depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }
        filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < max) ? 1 : 0);

    });

    for(size_t ii = 0; ii < filteringMask.size(); ++ii){
        if(filteringMask[ii] == 0){
            depthMask[ii] = 0;
        }
    }
}


auto K4Device::Impl::erode(uint8_t nbLoops, Connectivity connectivity) -> void{

    for(size_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        std::fill(filteringMask.begin(),filteringMask.end(), 0);

        for_each(std::execution::par_unseq, std::begin(indicesDepths1DNoBorders), std::end(indicesDepths1DNoBorders), [&](size_t id){

            if(depthMask[id] == 0){
                return;
            }

            uint8_t count = 0;
            if(connectivity == Connectivity::Connectivity_4){
                for(auto cId : indicesNeighbours4Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : indicesNeighbours8Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 8 ? 1 : 0;
            }
        });

        for(size_t ii = 0; ii < filteringMask.size(); ++ii){
            if(filteringMask[ii] == 0){
                depthMask[ii] = 0;
            }
        }
    }
}

auto K4Device::Impl::filter_color_image(const DCFilters &f) -> void{

    if(!depthSizedColorImage.has_value()){
        return;
    }

    if(!depthImage.has_value() && !infraredImage.has_value()){
        return;
    }

    Bench::start("[K4Device] Filter color");

    // retrieve buffers
    geo::Pt4<uint8_t>* colorBuffer = reinterpret_cast<geo::Pt4<uint8_t>*>(depthSizedColorImage->get_buffer());
    int16_t*  depthBuffer    = depthImage.has_value() ? reinterpret_cast<int16_t*>(depthImage.value().get_buffer()) : nullptr;
    uint16_t* infraredBuffer = infraredImage.has_value() ? reinterpret_cast<uint16_t*>(infraredImage.value().get_buffer()) : nullptr;

    static_cast<void>(infraredBuffer);

    for_each(std::execution::par_unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t id){
        if(f.invalidateColorFromDepth){
            if(depthBuffer[id] == dc_invalid_depth_value){
                colorBuffer[id] = dc_invalid_color_value;
            }else{
                if(f.keepOnlyBiggestCluster){
                    colorBuffer[meanBiggestZoneId] = {255,0,0,255};
                }
            }
        }
    });

    Bench::stop();
}

auto K4Device::Impl::filter_infrared_image(const DCFilters &f) -> void{

    if(!infraredImage.has_value()){
        return;
    }

    if(!depthSizedColorImage.has_value() && !depthImage.has_value()){
        return;
    }

    Bench::start("[K4Device] Filter infra");

    // retrieve buffers
    uint16_t* infraredBuffer = reinterpret_cast<uint16_t*>(infraredImage->get_buffer());
    geo::Pt4<uint8_t>* colorBuffer = depthSizedColorImage.has_value() ? reinterpret_cast<geo::Pt4<uint8_t>*>(depthSizedColorImage.value().get_buffer()) : nullptr;
    int16_t*  depthBuffer         = depthImage.has_value() ? reinterpret_cast<int16_t*>(depthImage.value().get_buffer()) : nullptr;

    static_cast<void>(colorBuffer);

    for_each(std::execution::par_unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t id){
        if(f.invalidateInfraFromDepth){
            if(depthBuffer[id] == dc_invalid_depth_value){
                infraredBuffer[id] = dc_invalid_infra_value;
            }
        }
    });

    Bench::stop();
}

auto K4Device::Impl::generate_cloud(const DCDataSettings &d, DCMode mode) -> void{

    if(has_cloud(mode) && pointCloudImage.has_value() && (d.sendCloud || d.generateCloudLocal)){
        Bench::start("[K4Device] Transformation depth_image_to_point_cloud");
        transformation.depth_image_to_point_cloud(depthImage.value(), K4A_CALIBRATION_TYPE_DEPTH, &pointCloudImage.value());
        Bench::stop();
    }
}

auto K4Device::Impl::compress_frame(const DCFilters &f, const DCDataSettings &d, DCMode mode) -> std::unique_ptr<K4CompressedFrame>{

    tool::Bench::start("[K4Device::compress_frame] Generate compressed frame");

    auto cFrame                = std::make_unique<K4CompressedFrame>();
    cFrame->mode               = mode;
    cFrame->idCapture          = static_cast<std::int32_t>(idCapture);
    cFrame->afterCaptureTS     = localTimestamps["after_capture"sv]->count();
    cFrame->validVerticesCount = validDepthValues;
    cFrame->calibration        = calibration;

    // compressed color
    if(colorImage.has_value() && d.sendColor){
        frameCompressor.add_color(
            colorImage->get_width_pixels(),
            colorImage->get_height_pixels(),
            4,
            colorImage.value().get_buffer(),
            f.jpegCompressionRate,
            cFrame.get()
        );
    }
    // compressed depth
    if(depthImage.has_value() && d.sendDepth){
        frameCompressor.add_depth(
            depthImage->get_width_pixels(),
            depthImage->get_height_pixels(),
            reinterpret_cast<std::uint16_t*>(depthImage.value().get_buffer()),
            cFrame.get()
        );
    }
    // compressed infrared
    if(infraredImage.has_value() && d.sendInfra){
        frameCompressor.add_depth(
            infraredImage->get_width_pixels(),
            infraredImage->get_height_pixels(),
            reinterpret_cast<std::uint16_t*>(infraredImage.value().get_buffer()),
            cFrame.get()
        );
    }

    // compressed cloud
    if(colorImage.has_value() && depthImage.has_value() && pointCloudImage.has_value() && d.sendCloud){
        frameCompressor.add_cloud(
            colorImage->get_width_pixels(), colorImage->get_height_pixels(), colorImage.value().get_buffer(),
            depthImage->get_width_pixels()*depthImage->get_height_pixels(), reinterpret_cast<std::uint16_t*>(depthImage.value().get_buffer()),
            f.jpegCompressionRate, cFrame.get()
        );
    }
    // uncompressed audio
    float *audioData = d.sendAudio ? reinterpret_cast<float*>(audioFrames.data()) : nullptr;
    size_t audioSize = lastFrameCount;
    if(audioData != nullptr && audioSize > 0){
        cFrame->audioFrames.resize(audioSize);
        std::copy(audioData, audioData + 7*audioSize, reinterpret_cast<float*>(cFrame->audioFrames.data()));
    }

    // uncompressed imu
    if(d.sendIMU){
        cFrame->imuSample = imuSample;
    }

    // uncompressed bodies
    if(d.sendBodies){
        // ...
    }

    tool::Bench::stop();

    return cFrame;
}

auto K4Device::Impl::create_local_frame(const DCDataSettings &d, DCMode mode) -> std::unique_ptr<DCFrame>{

    // write frame
    tool::Bench::start("[K4Device::create_local_frame] Write display data frame");

    auto dFrame = std::make_unique<DCFrame>();
    dFrame->idCapture      = static_cast<std::int32_t>(idCapture);
    dFrame->afterCaptureTS = localTimestamps["after_capture"sv]->count();

    dFrame->mode = mode;

    // init depth frame
    static constexpr std::array<Pt3f,5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    // color frame
    if(d.generateRGBLocalFrame && colorImage.has_value()){

        tool::Bench::start("[K4Device::create_local_frame] color");

        if(depthSizedColorImage.has_value()){
            dFrame->depthSizedColorWidth  = depthSizedColorImage->get_width_pixels();
            dFrame->depthSizedColorHeight = depthSizedColorImage->get_height_pixels();
            dFrame->depthSizedImageColorData.resize(dFrame->depthSizedColorWidth*dFrame->depthSizedColorHeight);

            auto colorBuffer = reinterpret_cast<const geo::Pt4<uint8_t>*>(depthSizedColorImage->get_buffer());
            for_each(std::execution::par_unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t id){
                dFrame->depthSizedImageColorData[id] = {
                    colorBuffer[id].z(),
                    colorBuffer[id].y(),
                    colorBuffer[id].x(),
                    255
                };
            });
        }

        dFrame->colorWidth  = colorImage->get_width_pixels();
        dFrame->colorHeight = colorImage->get_height_pixels();
        dFrame->imageColorData.resize(dFrame->colorWidth*dFrame->colorHeight);

        auto colorBuffer = reinterpret_cast<const geo::Pt4<uint8_t>*>(colorImage->get_buffer());
        for_each(std::execution::par_unseq, std::begin(indicesColors1D), std::end(indicesColors1D), [&](size_t id){
            dFrame->imageColorData[id] = {
                colorBuffer[id].z(),
                colorBuffer[id].y(),
                colorBuffer[id].x(),
                255
            };
        });

        tool::Bench::stop();
    }

    // depth frame
    if(d.generateDepthLocalFrame && depthImage.has_value()){

        tool::Bench::start("[K4Device::create_local_frame] depth");

        dFrame->depthWidth  = depthImage->get_width_pixels();
        dFrame->depthHeight = depthImage->get_height_pixels();
        dFrame->imageDepthData.resize(dFrame->depthWidth * dFrame->depthHeight);

        auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->get_buffer());
        const auto dRange = range(mode)*1000.f;
        const auto diff   = dRange.y() - dRange.x();

        for_each(std::execution::par_unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t id){

            if(depthBuffer[id] == dc_invalid_depth_value){
                dFrame->imageDepthData[id] = geo::Pt3<std::uint8_t>{};
                return;
            }

            float vF = (static_cast<float>(depthBuffer[id]) - dRange.x())/diff;
            float intPart;
            float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
            size_t idG = static_cast<size_t>(intPart);

            auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            dFrame->imageDepthData[id] = {
                static_cast<std::uint8_t>(255*col.x()),
                static_cast<std::uint8_t>(255*col.y()),
                static_cast<std::uint8_t>(255*col.z())
            };
        });

        tool::Bench::stop();
    }

    // infrared frame
    if(d.generateInfraLocalFrame && infraredImage.has_value()){

        tool::Bench::start("[K4Device::create_local_frame] display_infrared");

        dFrame->infraWidth  = infraredImage->get_width_pixels();;
        dFrame->infraHeight = infraredImage->get_height_pixels();;
        dFrame->imageInfraData.resize(dFrame->infraWidth * dFrame->infraHeight);

        auto infraBuffer = reinterpret_cast<const uint16_t*>(infraredImage->get_buffer());

        const float max = 2000;
        for_each(std::execution::par_unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t id){

            float vF = static_cast<float>(infraBuffer[id]);
            if(vF > max){
                vF = max;
            }
            vF/=max;
            dFrame->imageInfraData[id] = {
                static_cast<std::uint8_t>(255*vF),
                static_cast<std::uint8_t>(255*vF),
                static_cast<std::uint8_t>(255*vF)
            };
        });

        tool::Bench::stop();
    }

    // cloud
    if(d.generateCloudLocal && pointCloudImage.has_value() && depthSizedColorImage.has_value() && depthImage.has_value()){

        tool::Bench::start("[K4Device::create_local_frame] cloud");

        dFrame->cloud.vertices.resize(validDepthValues);
        dFrame->cloud.colors.resize(validDepthValues);        
        dFrame->cloud.normals.resize(validDepthValues);

        auto cloudBuffer = reinterpret_cast<geo::Pt3<int16_t>*>(pointCloudImage->get_buffer());
        auto colorBuffer = reinterpret_cast<const geo::Pt4<uint8_t>*>(depthSizedColorImage->get_buffer());
        auto depthBuffer = reinterpret_cast<const uint16_t*>(depthImage->get_buffer());

        for_each(std::execution::par_unseq, std::begin(indexDepthVertexCorrrespondance), std::end(indexDepthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(depthBuffer[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = std::get<1>(idC);
            dFrame->cloud.vertices[idV]= geo::Pt3f{
                static_cast<float>(-cloudBuffer[idD].x()),
                static_cast<float>(-cloudBuffer[idD].y()),
                static_cast<float>( cloudBuffer[idD].z())
            }*0.001f;
            dFrame->cloud.colors[idV] = geo::Pt3f{
                static_cast<float>(colorBuffer[idD].z()),
                static_cast<float>(colorBuffer[idD].y()),
                static_cast<float>(colorBuffer[idD].x())
            }/255.f;

            // A B C
            // D I E
            // F G H
            const auto &idN   = indicesNeighbours8Depth1D[idD];
            const auto &idDVC = indexDepthVertexCorrrespondance;
            const auto &v     = dFrame->cloud.vertices;
            Vec3f normal{};

            const auto &vId = v[idV];
            if(idN[0] != -1 && std::get<1>(idDVC[idN[0]]) != -1){
                if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vId x vIA
                    normal += cross(v[std::get<1>(idDVC[idN[3]])] - vId, v[std::get<1>(idDVC[idN[0]])] - vId);
                }
                if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIA x vIB
                    normal += cross(v[std::get<1>(idDVC[idN[0]])] - vId, v[std::get<1>(idDVC[idN[1]])] - vId);
                }
            }
            if(idN[2] != -1 && std::get<1>(idDVC[idN[2]]) != -1){
                if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIB x vIC
                    normal += cross(v[std::get<1>(idDVC[idN[1]])] - vId, v[std::get<1>(idDVC[idN[2]])] - vId);
                }
                if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIC x vIE
                    normal += cross(v[std::get<1>(idDVC[idN[2]])] - vId, v[std::get<1>(idDVC[idN[4]])] - vId);
                }
            }
            if(idN[7] != -1 && std::get<1>(idDVC[idN[7]]) != -1){
                if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIE x vIH
                    normal += cross(v[std::get<1>(idDVC[idN[4]])] - vId, v[std::get<1>(idDVC[idN[7]])] - vId);
                }
                if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIH x vIG
                    normal += cross(v[std::get<1>(idDVC[idN[7]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
                }
            }
            if(idN[5] != -1 && std::get<1>(idDVC[idN[5]]) != -1){
                if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIG x vIF
                    normal += cross(v[std::get<1>(idDVC[idN[5]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
                }
                if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vIF x vID
                    normal += cross(v[std::get<1>(idDVC[idN[6]])] - vId, v[std::get<1>(idDVC[idN[3]])] - vId);
                }
            }
            dFrame->cloud.normals[idV] = normalize(normal);
        });

        tool::Bench::stop();
    }

    // imu sample
    if(d.captureIMU){
        dFrame->imuSample = imuSample;
    }else{
        dFrame->imuSample = std::nullopt;
    }

    tool::Bench::stop();

    // send audio
    if(d.captureAudio && lastFrameCount != 0){

        tool::Bench::start("[K4Device::create_local_frame] audio");

        // copy audio frames
        dFrame->audioFrames.resize(lastFrameCount);
        auto audioFrom = reinterpret_cast<float*>(audioFrames.data());
        std::copy(audioFrom, audioFrom + 7*lastFrameCount, reinterpret_cast<float*>(dFrame->audioFrames.data()));

        tool::Bench::stop();
    }

    // send bodies
    if(d.captureBodies){
        dFrame->bodies.resize(bodiesCount);
        std::copy(bodies.begin(), bodies.begin() + bodiesCount, dFrame->bodies.begin());
    }

    return dFrame;
}


auto K4Device::Impl::init_data(DCMode mode) -> void{

    // init capture
    capture = std::make_unique<k4a::capture>();

    // init transform
    transformation = k4a::transformation(calibration);

    // reset images
    // # timestamps
    colorImageTS         = std::chrono::nanoseconds{0};
    depthImageTS         = std::chrono::nanoseconds{0};
    infraredImageTS      = std::chrono::nanoseconds{0};
    // # capture
    colorImage           = std::nullopt;
    depthImage           = std::nullopt;
    infraredImage        = std::nullopt;
    pointCloudImage      = std::nullopt;
    // # processing
    convertedColorImage  = std::nullopt;
    depthSizedColorImage = std::nullopt;

    // reset sizes
    idCapture            = 0;
    colorWidth           = 0;
    colorHeight          = 0;
    colorSize            = 0;
    depthWidth           = 0;
    depthHeight          = 0;
    depthSize            = 0;
    colorResolution      = color_resolution(mode);
    imageFormat          = image_format(mode);
    depthMode            = depth_mode(mode);

    if(colorResolution != DCColorResolution::OFF){

        // retrieve colors dimensions
        const auto colorDims     = k4a::GetColorDimensions(static_cast<k4a_color_resolution_t>(colorResolution));
        colorWidth  = std::get<0>(colorDims);
        colorHeight = std::get<1>(colorDims);
        colorSize   = colorWidth*colorHeight;

        if(imageFormat == DCImageFormat::YUY2 || imageFormat == DCImageFormat::NV12 || imageFormat == DCImageFormat::MJPG){
            convertedColorImage = k4a::image::create(K4A_IMAGE_FORMAT_COLOR_BGRA32,
                static_cast<int>(colorWidth),
                static_cast<int>(colorHeight),
                static_cast<int32_t>(colorWidth * 4 * sizeof(uint8_t))
            );
        }

        // set color indices
        indicesColors1D.resize(colorSize);
        std::iota(std::begin(indicesColors1D), std::end(indicesColors1D), 0);
    }


    if(depthMode != DCDepthMode::K4_OFF){

        // retrieve depth dimensions
        auto depthRes = depth_resolution(mode);
        depthWidth  = depthRes.x();
        depthHeight = depthRes.y();
        depthSize   = depthWidth*depthHeight;

        // init resized color image
        if(colorResolution != DCColorResolution::OFF){
            depthSizedColorImage = k4a::image::create(K4A_IMAGE_FORMAT_COLOR_BGRA32,
                static_cast<int>(depthWidth),
                static_cast<int>(depthHeight),
                static_cast<int32_t>(depthWidth * 4 * sizeof(uint8_t))
            );
        }

        // set depth indices
        depthMask.resize(depthSize);
        filteringMask.resize(depthSize);
        depthFiltering.resize(depthSize);
        zonesId.resize(depthSize);
        indicesDepths1D.resize(depthSize);
        indexDepthVertexCorrrespondance.resize(depthSize);


        indicesDepthsSortedCorrespondanceNoBorders.resize((depthWidth-2)*(depthHeight-2));

        std::iota(std::begin(indicesDepths1D), std::end(indicesDepths1D), 0);
        indicesDepths3D.resize(depthSize);

        indicesDepths1DNoBorders.clear();        
        indicesDepths1DNoBorders.reserve((depthWidth-2)*(depthHeight-2));

        indicesNeighbours4Depth1D.clear();
        indicesNeighbours4Depth1D.reserve(depthSize);

        indicesNeighbours8Depth1D.clear();
        indicesNeighbours8Depth1D.reserve(depthSize);

        size_t id = 0;
        for(size_t ii = 0; ii < depthHeight; ++ii){
            for(size_t jj = 0; jj < depthWidth; ++jj){

                indicesDepths3D[id] = {id,jj,ii};

                // A B C
                // D I E
                // F G H
                std::int32_t idA = -1;
                std::int32_t idD = -1;
                std::int32_t idF = -1;
                std::int32_t idC = -1;
                std::int32_t idE = -1;
                std::int32_t idH = -1;
                std::int32_t idB = -1;
                std::int32_t idG = -1;

                bool notOnLeft   = jj > 0;
                bool notOnRight  = jj < depthWidth - 1;
                bool notOnTop    = ii > 0;
                bool notOnBottom = ii < depthHeight-1;

                if(notOnLeft){
                    idD = id - 1;
                    if(notOnTop){
                        idA = id - depthWidth-1;
                    }
                    if(notOnBottom){
                        idF = id + depthWidth-1;
                    }
                }
                if(notOnRight){
                    idE = id + 1;
                    if(notOnTop){
                        idC = id - depthWidth + 1;
                    }
                    if(notOnBottom){
                        idH = id + depthWidth + 1;
                    }
                }
                if(notOnTop){
                    idB = id - depthWidth;
                }
                if(notOnBottom){
                    idG = id + depthWidth;
                }

                indicesNeighbours4Depth1D.push_back({idB,idD,idE,idG});
                indicesNeighbours8Depth1D.push_back({idA,idB,idC,idD,idE,idF,idG,idH});

                if(notOnLeft && notOnRight && notOnTop && notOnBottom){
                    indicesDepths1DNoBorders.push_back(id);
                }

                ++id;
            }
        }

        // init cloud image
        if(has_cloud(mode)){
            pointCloudImage = k4a::image::create(K4A_IMAGE_FORMAT_CUSTOM,
                static_cast<int>(depthWidth),
                static_cast<int>(depthHeight),
                static_cast<int32_t>(depthWidth * 3 * sizeof(int16_t))
            );
        }
    }
}


auto K4Device::Impl::read_from_microphones() -> void{

    lastFrameCount = 0;
    if(audioListener != nullptr){

        // process audio frame
        audioListener->ProcessFrames([&](k4a::K4AMicrophoneFrame *frame, const size_t frameCount) {

            // store last count
            lastFrameCount = frameCount;
            if(lastFrameCount == 0){
                return lastFrameCount;
            }

            // resize audio buffer
            if(audioFrames.size() < lastFrameCount){
                audioFrames.resize(lastFrameCount);
            }

            // copy data
            std::copy(frame, frame + lastFrameCount, audioFrames.begin());

            return lastFrameCount;
        });

        if (audioListener->GetStatus() != SoundIoErrorNone){
            Logger::error(std::format("[K4Device] Error while recording {}\n", soundio_strerror(audioListener->GetStatus())));
        }else if (audioListener->Overflowed()){
            Logger::warning(std::format("[K4Device] Warning: sound overflow detected!\n"));
            audioListener->ClearOverflowed();
        }
    }
}

auto K4Device::Impl::read_from_imu() -> void{
    k4a_imu_sample_t sample;
    if(device.get_imu_sample(&sample, std::chrono::milliseconds(1))){
        imuSample.temperature = sample.temperature;
        const auto &dAcc = sample.acc_sample.xyz;
        imuSample.acc ={dAcc.x,dAcc.y,dAcc.z};
        imuSample.accTsMs = sample.acc_timestamp_usec;
        const auto &dGyr = sample.gyro_sample.xyz;
        imuSample.gyr = {dGyr.x,dGyr.y,dGyr.z};
        imuSample.gyrTsMs = sample.gyro_timestamp_usec;
        kinect4->new_imu_sample_signal(imuSample);
    }
}

auto K4Device::Impl::get_color_image() -> bool{
    if(colorResolution != DCColorResolution::OFF){

        Bench::start("[K4Device] Capture get_color_image");
        colorImage = capture->get_color_image();
        Bench::stop();

        if (!colorImage->is_valid()){
            Logger::error("[K4Device] Failed to get color image from capture\n");
            return false;
        }

        colorImageTS = colorImage->get_system_timestamp();
    }
    return true;
}

auto K4Device::Impl::get_depth_image() -> bool{

    if(depthMode != DCDepthMode::K4_OFF){

        Bench::start("[K4Device] Capture get_depth_image");
        depthImage = capture->get_depth_image();        
        Bench::stop();

        if (!depthImage->is_valid()){
            Logger::error("[K4Device] Failed to get depth image from capture\n");
            return false;
        }

        depthImageTS = depthImage->get_system_timestamp();
    }

    return true;
}

auto K4Device::Impl::get_infra_image(DCMode mode) -> bool{

    if(has_infrared(mode)){

        Bench::start("[K4Device] Capture get_ir_image");
        infraredImage = capture->get_ir_image();        
        Bench::stop();

        if (!infraredImage->is_valid()){
            Logger::error("[K4Device] Failed to get infrared image from capture\n");
            return false;
        }

        infraredImageTS = infraredImage->get_system_timestamp();
    }

    return true;
}


auto K4Device::Impl::convert_color_image(const DCFilters &f) -> void{

    if(colorResolution == DCColorResolution::OFF){
        return;
    }

    if(imageFormat == DCImageFormat::NV12){

        // libyuv refers to pixel order in system-endian order but OpenGL refers to
        // pixel order in big-endian order, which is why we create the OpenGL texture
        // as "RGBA" but then use the "ABGR" libyuv function here.

        const int luminanceStride  = colorImage->get_width_pixels();
        const int hueSatStride     = colorImage->get_width_pixels();
        const uint8_t *hueSatStart = colorImage->get_buffer() + luminanceStride * colorImage->get_height_pixels();

        Bench::start("[K4Device] NV12 convert");
        int result = libyuv::NV12ToARGB(
            colorImage->get_buffer(),                                               // src_y
            luminanceStride,                                                        // src_stride_y
            hueSatStart,                                                            // src_vu
            hueSatStride,                                                           // src_stride_vu
            convertedColorImage->get_buffer(),                                      // dst_argb
            colorImage->get_width_pixels() * static_cast<int>(sizeof(geo::Pt4<std::uint8_t>)),  // dst_stride_argb
            colorImage->get_width_pixels(),                                                     // width
            colorImage->get_height_pixels()                                                     // height
        );
        Bench::stop();

        if (result != 0){
            Logger::error("[K4Device::Impl::convert_color_image] libyuv::NV12ToARGB error.\n");
            return;
        }

        colorImage = convertedColorImage;

    } else if(imageFormat == DCImageFormat::YUY2 ){ // //YUY2 is a 4:2:2 format, so there are 4 bytes per 'chunk' of data, and each 'chunk' represents 2 pixels.

        Bench::start("[K4Device] YUY2 convert");
        const int stride = colorImage->get_width_pixels() * 4 / 2;
        int result = libyuv::YUY2ToARGB(
            colorImage->get_buffer(),                                                           // src_yuy2,
            stride,                                                                             // src_stride_yuy2,
            convertedColorImage->get_buffer(),                                                  // dst_argb,
            colorImage->get_width_pixels() * static_cast<int>(sizeof(geo::Pt4<std::uint8_t>)),  // dst_stride_argb
            colorImage->get_width_pixels(),                                                     // width,
            colorImage->get_height_pixels()                                                     // height
        );
        Bench::stop();

        if (result != 0){
            Logger::error("[K4Device::Impl::convert_color_image] libyuv::YUY2ToARGB error.\n");
            return;
        }

        colorImage = convertedColorImage;

    }else if(imageFormat == DCImageFormat::MJPG){

        frameUncompressor.uncompress_jpeg_data(
            colorImage->get_width_pixels(),
            colorImage->get_height_pixels(),
            DCFrameUncompressor::ColorFormat::BGRA,
            colorImage->get_size(),
            colorImage->get_buffer(),
            convertedColorImage->get_buffer()
        );

        colorImage = convertedColorImage;

    }else if(imageFormat == DCImageFormat::BGRA){
        // nothing to do        
    }
}

auto K4Device::Impl::resize_color_to_fit_depth() -> void{

    if(colorImage.has_value() && depthImage.has_value()){

        Bench::start("[K4Device] Transformation color_image_to_depth_camera");
        transformation.color_image_to_depth_camera(
            depthImage.value(),
            colorImage.value(),
            &depthSizedColorImage.value());
        Bench::stop();
    }
}



